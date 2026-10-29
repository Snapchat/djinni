/**
  * Copyright 2014 Dropbox, Inc.
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *    http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  * This file has been modified by Snap, Inc.
  */

package djinni

import java.io.{BufferedWriter, File, FileNotFoundException, FileWriter, IOException}
import java.io.{FileInputStream, InputStreamReader}

import org.yaml.snakeyaml.Yaml
import scala.collection.JavaConverters._
import scala.util.control.NonFatal

import djinni.generatorTools._

object Main {

  private val operationalConfigFlags = Set(
    "--idl", "--idl-include-path", "--java-out", "--cpp-out", "--cpp-header-out",
    "--jni-out", "--jni-header-out", "--objc-out", "--objcpp-out", "--wasm-out",
    "--ts-out", "--swift-out", "--swiftxx-out", "--valdi-out", "--valdi-ts-out",
    "--c-out", "--c-header-out", "--yaml-out", "--list-in-files", "--list-out-files",
    "--list-declarations", "--verify-declarations", "--skip-generation"
  )

  private def configMap(value: Any, context: String): Seq[(String, Any)] = value match {
    case map: java.util.Map[_, _] =>
      map.asScala.toSeq.map {
        case (key: String, item) => (key, item)
        case (key, _) => throw new IllegalArgumentException(context + " keys must be strings; found " + key)
      }
    case map: scala.collection.Map[_, _] =>
      map.toSeq.map {
        case (key: String, item) => (key, item)
        case (key, _) => throw new IllegalArgumentException(context + " keys must be strings; found " + key)
      }
    case _ => throw new IllegalArgumentException(context + " must be a YAML mapping")
  }

  private def readConfig(file: File): Any = {
    val in = new InputStreamReader(new FileInputStream(file), "UTF-8")
    try new Yaml().load[Any](in) finally in.close()
  }

  private def mergeConfigValues(base: Any, overrideValue: Any): Any = {
    def entries(value: Any): Option[Map[String, Any]] = value match {
      case _: java.util.Map[_, _] | _: scala.collection.Map[_, _] => Some(configMap(value, "configuration").toMap)
      case _ => None
    }
    (entries(base), entries(overrideValue)) match {
      case (Some(baseMap), Some(overrideMap)) =>
        (baseMap.keySet ++ overrideMap.keySet).map { key =>
          val value = (baseMap.get(key), overrideMap.get(key)) match {
            case (Some(baseValue), Some(overrideValue)) => mergeConfigValues(baseValue, overrideValue)
            case (Some(baseValue), None) => baseValue
            case (None, Some(overrideValue)) => overrideValue
            case _ => throw new AssertionError("unreachable")
          }
          key -> value
        }.toMap
      case _ => overrideValue
    }
  }

  private def validateConfigMappings(value: Any, context: String): Unit = {
    configMap(value, context).foreach { case (key, nested) =>
      if (nested.isInstanceOf[java.util.Map[_, _]] || nested.isInstanceOf[scala.collection.Map[_, _]]) {
        validateConfigMappings(nested, context + "." + key)
      }
    }
  }

  private def readConfigArgs(config: Any): Array[String] = {
    validateConfigMappings(config, "configuration")
    val rootEntries = configMap(config, "configuration")
    if (rootEntries.map(_._1).distinct.length != rootEntries.length) {
      throw new IllegalArgumentException("configuration contains duplicate keys")
    }
    val rootMap = rootEntries.toMap
    rootMap.get("schema_version") match {
      case Some(version: java.lang.Number) if version.intValue() == 1 =>
      case Some(version) => throw new IllegalArgumentException("unsupported schema_version: " + version)
      case None => throw new IllegalArgumentException("configuration requires schema_version: 1")
    }
    val extraRootKeys = rootMap.keySet -- Set("schema_version", "languages")
    if (extraRootKeys.nonEmpty) throw new IllegalArgumentException("unknown configuration keys: " + extraRootKeys.toSeq.sorted.mkString(", "))

    val languages = rootMap.getOrElse("languages", throw new IllegalArgumentException("configuration requires languages"))
    val languageEntries = configMap(languages, "languages")
    if (languageEntries.map(_._1).distinct.length != languageEntries.length) {
      throw new IllegalArgumentException("languages contains duplicate keys")
    }
    val args = scala.collection.mutable.ArrayBuffer[String]()
    languageEntries.foreach { case (language, settings) =>
      val settingEntries = configMap(settings, "languages." + language)
      if (settingEntries.map(_._1).distinct.length != settingEntries.length) {
        throw new IllegalArgumentException("languages." + language + " contains duplicate keys")
      }
      settingEntries.foreach {
        case ("identifiers", identifiers) =>
          val identifierEntries = configMap(identifiers, "languages." + language + ".identifiers")
          if (identifierEntries.map(_._1).distinct.length != identifierEntries.length) {
            throw new IllegalArgumentException("languages." + language + ".identifiers contains duplicate keys")
          }
          identifierEntries.foreach { case (name, value) =>
            appendConfigOption(args, "--ident-" + language + "-" + name.replace('_', '-'), value)
          }
        case ("output_file", value) if language == "yaml" => appendConfigOption(args, "--yaml-out-file", value)
        case (name, value) =>
          appendConfigOption(args, "--" + language + "-" + name.replace('_', '-'), value)
      }
    }
    args.toArray
  }

  private def appendConfigOption(args: scala.collection.mutable.ArrayBuffer[String], flag: String, value: Any): Unit = value match {
    case text: String => args ++= Seq(flag, text)
    case boolean: java.lang.Boolean if flag == "--swift-private-in-same-module" && boolean.booleanValue() => args += flag
    case boolean: java.lang.Boolean if flag == "--swift-private-in-same-module" =>
      throw new IllegalArgumentException(flag + " is a presence-only option and cannot be false")
    case boolean: java.lang.Boolean => args ++= Seq(flag, boolean.toString)
    case other => throw new IllegalArgumentException(flag + " must have a string or boolean value; found " + other)
  }

  private def extractConfigArguments(args: Array[String]): (Option[File], Option[File], Boolean, Array[String]) = {
    var defaults: Option[File] = None
    var config: Option[File] = None
    var verify = false
    val remaining = scala.collection.mutable.ArrayBuffer[String]()
    var index = 0
    while (index < args.length) {
      args(index) match {
        case "--defaults" =>
          if (index + 1 >= args.length) throw new IllegalArgumentException("--defaults requires a file path")
          if (defaults.isDefined) throw new IllegalArgumentException("--defaults may only be specified once")
          defaults = Some(new File(args(index + 1)))
          index += 2
        case "--config" =>
          if (index + 1 >= args.length) throw new IllegalArgumentException("--config requires a file path")
          if (config.isDefined) throw new IllegalArgumentException("--config may only be specified once")
          config = Some(new File(args(index + 1)))
          index += 2
        case "--verify-config" =>
          verify = true
          index += 1
        case other =>
          remaining += other
          index += 1
      }
    }
    (defaults, config, verify, remaining.toArray)
  }

  private def optionValues(args: Array[String]): Map[String, String] = {
    val values = scala.collection.mutable.Map[String, String]()
    val presenceOnly = Set("--swift-private-in-same-module")
    var index = 0
    while (index < args.length) {
      val flag = args(index)
      val hasValue = index + 1 < args.length && !args(index + 1).startsWith("--")
      if (presenceOnly.contains(flag)) {
        values(flag) = "<enabled>"
        index += 1
      } else if (!operationalConfigFlags.contains(flag)) {
        if (hasValue) {
          values(flag) = args(index + 1)
          index += 2
        } else {
          values(flag) = "<enabled>"
          index += 1
        }
      } else if (hasValue) {
        index += 2
      } else {
        index += 1
      }
    }
    values.toMap
  }

  private def mergeConfigAndCommandLine(configArgs: Array[String], cliArgs: Array[String]): Array[String] = {
    val commandLineFlags = cliArgs.filter(_.startsWith("--")).toSet
    val merged = scala.collection.mutable.ArrayBuffer[String]()
    var index = 0
    while (index < configArgs.length) {
      val flag = configArgs(index)
      val hasValue = index + 1 < configArgs.length && !configArgs(index + 1).startsWith("--")
      if (!commandLineFlags.contains(flag)) {
        merged += flag
        if (hasValue) merged += configArgs(index + 1)
      }
      index += (if (hasValue) 2 else 1)
    }
    merged ++= cliArgs
    merged.toArray
  }

  private def operationalArguments(args: Array[String]): Array[String] = {
    val result = scala.collection.mutable.ArrayBuffer[String]()
    var index = 0
    while (index < args.length) {
      val flag = args(index)
      val hasValue = index + 1 < args.length && !args(index + 1).startsWith("--")
      if (operationalConfigFlags.contains(flag)) {
        result += flag
        if (hasValue) result += args(index + 1)
      }
      index += (if (hasValue) 2 else 1)
    }
    result.toArray
  }

  private def verifyConfigMatches(configArgs: Array[String], cliArgs: Array[String]): Unit = {
    val fromConfig = optionValues(configArgs)
    val fromCli = optionValues(cliArgs)
    val keys = fromConfig.keySet ++ fromCli.keySet
    val mismatches = keys.toSeq.sorted.flatMap { key =>
      val configured = fromConfig.get(key)
      val commandLine = fromCli.get(key)
      if (configured == commandLine) None
      else Some("  " + key + ": config=" + configured.getOrElse("<missing>") + ", command line=" + commandLine.getOrElse("<missing>"))
    }
    if (mismatches.nonEmpty) {
      throw new IllegalArgumentException("configuration does not match command-line settings:\n" + mismatches.mkString("\n"))
    }
  }

  def main(args: Array[String]) {
    val (defaultsFile, explicitConfigFile, verifyConfig, cliArgs) = try extractConfigArguments(args) catch {
      case NonFatal(ex) => System.err.println("Configuration error: " + ex.getMessage); System.exit(1); return
    }
    if (verifyConfig && defaultsFile.isEmpty && explicitConfigFile.isEmpty) {
      System.err.println("--verify-config requires --defaults or --config")
      System.exit(1)
      return
    }
    val configArgs = try {
      val mergedConfig = Seq(defaultsFile, explicitConfigFile).flatten.foldLeft(Option.empty[Any]) { (current, file) =>
        Some(current.map(previous => mergeConfigValues(previous, readConfig(file))).getOrElse(readConfig(file)))
      }
      mergedConfig.map(readConfigArgs).getOrElse(Array[String]())
    } catch {
      case NonFatal(ex) => System.err.println("Error reading configuration file: " + ex.getMessage); System.exit(1); return
    }
    if (verifyConfig) {
      try verifyConfigMatches(configArgs, cliArgs) catch {
        case NonFatal(ex) => System.err.println(ex.getMessage); System.exit(1); return
      }
    }
    val parseArgs = if (verifyConfig) {
      operationalArguments(cliArgs) ++ configArgs
    } else {
      mergeConfigAndCommandLine(configArgs, cliArgs)
    }
    var idlFile: File = null
    var idlIncludePaths: List[String] = List("")
    var cppOutFolder: Option[File] = None
    var cppNamespace: String = ""
    var cppIncludePrefix: String = ""
    var cppExtendedRecordIncludePrefix: String = ""
    var cppFileIdentStyle: IdentConverter = IdentStyle.underLower
    var cppBaseLibIncludePrefix: String = ""
    var cppOptionalTemplate: String = "std::optional"
    var cppOptionalHeader: String = "<optional>"
    var cppNulloptValue: String = "std::nullopt"
    var cppEnumHashWorkaround : Boolean = true
    var cppNnHeader: Option[String] = None
    var cppNnType: Option[String] = None
    var cppNnCheckExpression: Option[String] = None
    var cppUseWideStrings: Boolean = false
    var cppLegacyRecords: Boolean = false
    var javaOutFolder: Option[File] = None
    var javaPackage: Option[String] = None
    var javaClassAccessModifier: JavaAccessModifier.Value = JavaAccessModifier.Public
    var javaCppException: Option[String] = None
    var javaAnnotation: Option[String] = None
    var javaNullableAnnotation: Option[String] = None
    var javaNonnullAnnotation: Option[String] = None
    var javaImplementAndroidOsParcelable : Boolean = false
    var javaUseFinalForRecord: Boolean = true
    var javaLegacyRecords: Boolean = false
    var javaGenInterface: Boolean = false
    var jniOutFolder: Option[File] = None
    var jniHeaderOutFolderOptional: Option[File] = None
    var jniNamespace: String = "djinni_generated"
    var jniClassIdentStyleOptional: Option[IdentConverter] = None
    var jniIncludePrefix: String = ""
    var jniIncludeCppPrefix: String = ""
    var jniFileIdentStyleOptional: Option[IdentConverter] = None
    var jniBaseLibClassIdentStyleOptional: Option[IdentConverter] = None
    var jniBaseLibIncludePrefix: String = ""
    var jniUseOnLoad: Boolean = false
    var jniFunctionPrologueFile: Option[String] = None
    var cppHeaderOutFolderOptional: Option[File] = None
    var cppExt: String = "cpp"
    var cppHeaderExt: String = "hpp"
    var javaIdentStyle = IdentStyle.javaDefault
    var cppIdentStyle = IdentStyle.cppDefault
    var cppTypeEnumIdentStyle: IdentConverter = null
    var objcOutFolder: Option[File] = None
    var objcppOutFolder: Option[File] = None
    var objcppExt: String = "mm"
    var objcHeaderExt: String = "h"
    var objcIdentStyle = IdentStyle.objcDefault
    var objcTypePrefix: String = ""
    var objcIncludePrefix: String = ""
    var objcExtendedRecordIncludePrefix: String = ""
    var objcSwiftBridgingHeaderName: Option[String] = None
    var objcGenProtocol: Boolean = false
    var objcDisableClassCtor: Boolean = false
    var objcClosedEnums: Boolean = false
    var objcppIncludePrefix: String = ""
    var objcppIncludeCppPrefix: String = ""
    var objcppIncludeObjcPrefixOptional: Option[String] = None
    var objcppFunctionPrologueFile: Option[String] = None
    var objcppDisableExceptionTranslation: Boolean = false
    var objcFileIdentStyleOptional: Option[IdentConverter] = None
    var objcStrictProtocol: Boolean = true
    var objcLegacyRecords: Boolean = false
    var objcOmitFullConvenienceConstructor: Boolean = false
    var objcppNamespace: String = "djinni_generated"
    var objcBaseLibIncludePrefix: String = ""
    var wasmOutFolder: Option[File] = None
    var wasmIncludePrefix: String = ""
    var wasmIncludeCppPrefix: String = ""
    var wasmBaseLibIncludePrefix: String = ""
    var wasmOmitConstants: Boolean = false
    var wasmNamespace: Option[String] = None
    var wasmOmitNsAlias: Boolean = false
    var wasmCppNamespace: Option[String] = None
    var wasmClassIdentStyleOptional: Option[IdentConverter] = None
    var wasmFileIdentStyleOptional: Option[IdentConverter] = None
    var valdiNamespace: String = "djinni_generated"
    var valdiClassIdentStyleOptional: Option[IdentConverter] = None
    var valdiFileIdentStyleOptional: Option[IdentConverter] = None
    var valdiTsOutFolder: Option[File] = None
    var jsIdentStyle = IdentStyle.jsDefault
    var tsOutFolder: Option[File] = None
    var tsModule: String = "module"
    var valdiOutFolder: Option[File] = None
    var valdiIncludePrefix: String = ""
    var valdiIncludeCppPrefix: String = ""
    var valdiBaseLibIncludePrefix: String = ""
    var cOutFolder: Option[File] = None
    var cHeaderOutFolderOptional: Option[File] = None
    var cNamespace: String = ""
    var cBaseLibIncludePrefix: String = ""
    var cIncludePrefix: String = ""
    var cWrapperCppNamespace: Option[String] = None
    var cWrapperUseDlsym: Boolean = false
    var swiftOutFolder: Option[File] = None
    var swiftModule: String = "Module"
    var swiftPrivateInSameModule: Boolean = false
    var swiftIdentStyle = IdentStyle.swiftDefault
    var swiftxxOutFolder: Option[File] = None
    var swiftxxNamespace: String = "djinni_generated"
    var swiftxxIncludePrefix: String = ""
    var swiftxxBaseLibModule: String = "DjinniSupportCxx"
    var swiftxxClassIdentStyleOptional: Option[IdentConverter] = None
    var swiftxxFileIdentStyleOptional: Option[IdentConverter] = None
    var swiftxxIncludeCppPrefix: String = ""
    var swiftxxBaseLibIncludePrefix: String = ""
    var inFileListPath: Option[File] = None
    var outFileListPath: Option[File] = None
    var declarationManifestPath: Option[File] = None
    var verifyDeclarationManifestPath: Option[File] = None
    var skipGeneration: Boolean = false
    var yamlOutFolder: Option[File] = None
    var yamlOutFile: Option[String] = None
    var yamlPrefix: String = ""

    val argParser = new scopt.OptionParser[Unit]("djinni") {

      def identStyle(optionName: String, update: IdentConverter => Unit) = {
        opt[String](optionName).valueName("...").foreach(spec =>
          IdentStyle.infer(spec) match {
            case None => failure("invalid ident spec: \"" + spec + "\"")
            case Some(func) => update(func)
          }
        )
      }

      override def showUsageOnError = false
      help("help")
      opt[File]("idl").valueName("<in-file>").required().foreach(idlFile = _)
        .text("The IDL file with the type definitions, typically with extension \".djinni\".")
      opt[String]("idl-include-path").valueName("<path> ...").optional().unbounded().foreach(x => idlIncludePaths = idlIncludePaths :+ x)
        .text("An include path to search for Djinni @import directives. Can specify multiple paths.")
      note("")
      opt[File]("java-out").valueName("<out-folder>").foreach(x => javaOutFolder = Some(x))
        .text("The output for the Java files (Generator disabled if unspecified).")
      opt[String]("java-package").valueName("...").foreach(x => javaPackage = Some(x))
        .text("The package name to use for generated Java classes.")
      opt[JavaAccessModifier.Value]("java-class-access-modifier").valueName("<public/package>").foreach(x => javaClassAccessModifier = x)
        .text("The access modifier to use for generated Java classes (default: public).")
      opt[String]("java-cpp-exception").valueName("<exception-class>").foreach(x => javaCppException = Some(x))
        .text("The type for translated C++ exceptions in Java (default: java.lang.RuntimeException that is not checked)")
      opt[String]("java-annotation").valueName("<annotation-class>").foreach(x => javaAnnotation = Some(x))
        .text("Java annotation (@Foo) to place on all generated Java classes")
      opt[String]("java-nullable-annotation").valueName("<nullable-annotation-class>").foreach(x => javaNullableAnnotation = Some(x))
        .text("Java annotation (@Nullable) to place on all fields and return values that are optional")
      opt[String]("java-nonnull-annotation").valueName("<nonnull-annotation-class>").foreach(x => javaNonnullAnnotation = Some(x))
        .text("Java annotation (@Nonnull) to place on all fields and return values that are not optional")
      opt[Boolean]("java-implement-android-os-parcelable").valueName("<true/false>").foreach(x => javaImplementAndroidOsParcelable = x)
        .text("all generated java classes will implement the interface android.os.Parcelable")
      opt[Boolean]("java-use-final-for-record").valueName("<use-final-for-record>").foreach(x => javaUseFinalForRecord = x)
        .text("Whether generated Java classes for records should be marked 'final' (default: true). ")
      opt[Boolean]("java-legacy-records").valueName("<legacy-records>").foreach(x => javaLegacyRecords = x)
        .text("Use legacy record behavior for Java code (default: false)")
      opt[Boolean]("java-gen-interface").valueName("<true/false>").foreach(x => javaGenInterface = x)
        .text("Generate Java interface instead of abstract class.")
      note("")
      opt[File]("cpp-out").valueName("<out-folder>").foreach(x => cppOutFolder = Some(x))
        .text("The output folder for C++ files (Generator disabled if unspecified).")
      opt[File]("cpp-header-out").valueName("<out-folder>").foreach(x => cppHeaderOutFolderOptional = Some(x))
        .text("The output folder for C++ header files (default: the same as --cpp-out).")
      opt[String]("cpp-include-prefix").valueName("<prefix>").foreach(cppIncludePrefix = _)
        .text("The prefix for #includes of header files from C++ files.")
      opt[String]("cpp-base-lib-include-prefix").valueName("...").foreach(x => cppBaseLibIncludePrefix = x)
        .text("The C++ base library's include path, relative to the C++ classes.")
      opt[String]("cpp-namespace").valueName("...").foreach(x => cppNamespace = x)
        .text("The namespace name to use for generated C++ classes.")
      opt[String]("cpp-ext").valueName("<ext>").foreach(cppExt = _)
        .text("The filename extension for C++ files (default: \"cpp\").")
      opt[String]("hpp-ext").valueName("<ext>").foreach(cppHeaderExt = _)
        .text("The filename extension for C++ header files (default: \"hpp\").")
      opt[String]("cpp-optional-template").valueName("<template>").foreach { x =>
        System.err.println("Warning: --cpp-optional-template is deprecated. std::optional is standard in C++17+.")
        cppOptionalTemplate = x
      }.text("DEPRECATED: The template to use for optional values (default: \"std::optional\")")
      opt[String]("cpp-optional-header").valueName("<header>").foreach { x =>
        System.err.println("Warning: --cpp-optional-header is deprecated. std::optional is standard in C++17+.")
        cppOptionalHeader = x
      }.text("DEPRECATED: The header to use for optional values (default: \"<optional>\")")
      opt[String]("cpp-nullopt-value").valueName("<value>").foreach { x =>
        System.err.println("Warning: --cpp-nullopt-value is deprecated. std::optional is standard in C++17+.")
        cppNulloptValue = x
      }.text("DEPRECATED: The value to use for nullopt defaults of optional values (default: \"std::nullopt\")")
      opt[Boolean]("cpp-enum-hash-workaround").valueName("<true/false>").foreach(x => cppEnumHashWorkaround = x)
        .text("Work around LWG-2148 by generating std::hash specializations for C++ enums (default: true)")
      opt[String]("cpp-nn-header").valueName("<header>").foreach(x => cppNnHeader = Some(x))
        .text("The header to use for non-nullable pointers")
      opt[String]("cpp-nn-type").valueName("<header>").foreach(x => cppNnType = Some(x))
        .text("The type to use for non-nullable pointers (as a substitute for std::shared_ptr)")
      opt[String]("cpp-nn-check-expression").valueName("<header>").foreach(x => cppNnCheckExpression = Some(x))
        .text("The expression to use for building non-nullable pointers")
      opt[Boolean]( "cpp-use-wide-strings").valueName("<true/false>").foreach(x => cppUseWideStrings = x)
        .text("Use wide strings in C++ code (default: false)")
      opt[Boolean]( "cpp-legacy-records").valueName("<true/false>").foreach(x => cppLegacyRecords = x)
        .text("Use legacy record behavior for C++ code (default: false)")
      note("")
      opt[File]("jni-out").valueName("<out-folder>").foreach(x => jniOutFolder = Some(x))
        .text("The folder for the JNI C++ output files (Generator disabled if unspecified).")
      opt[File]("jni-header-out").valueName("<out-folder>").foreach(x => jniHeaderOutFolderOptional = Some(x))
        .text("The folder for the JNI C++ header files (default: the same as --jni-out).")
      opt[String]("jni-include-prefix").valueName("<prefix>").foreach(jniIncludePrefix = _)
        .text("The prefix for #includes of JNI header files from JNI C++ files.")
      opt[String]("jni-include-cpp-prefix").valueName("<prefix>").foreach(jniIncludeCppPrefix = _)
        .text("The prefix for #includes of the main header files from JNI C++ files.")
      opt[String]("jni-namespace").valueName("...").foreach(x => jniNamespace = x)
        .text("The namespace name to use for generated JNI C++ classes.")
      opt[String]("jni-base-lib-include-prefix").valueName("...").foreach(x => jniBaseLibIncludePrefix = x)
        .text("The JNI base library's include path, relative to the JNI C++ classes.")
      opt[Boolean]("jni-use-on-load-initializer").valueName("<true/false>").foreach(x => jniUseOnLoad = x)
        .text("If true, djinni will use RegisterNativeMethods to bind JNI functions, instead of the Java_* style symbol exports")
      opt[String]("jni-function-prologue-file").valueName("<header-file>").foreach(x => jniFunctionPrologueFile = Some(x))
        .text("User header file to include in generated JNI C++ classes.")
      note("")
      opt[File]("objc-out").valueName("<out-folder>").foreach(x => objcOutFolder = Some(x))
        .text("The output folder for Objective-C files (Generator disabled if unspecified).")
      opt[String]("objc-h-ext").valueName("<ext>").foreach(objcHeaderExt = _)
        .text("The filename extension for Objective-C[++] header files (default: \"h\")")
      opt[String]("objc-type-prefix").valueName("<pre>").foreach(objcTypePrefix = _)
        .text("The prefix for Objective-C data types (usually two or three letters)")
      opt[String]("objc-include-prefix").valueName("<prefix>").foreach(objcIncludePrefix = _)
        .text("The prefix for #import of header files from Objective-C files.")
      opt[String]("objc-swift-bridging-header").valueName("<name>").foreach(x => objcSwiftBridgingHeaderName = Some(x))
        .text("The name of Objective-C Bridging Header used in XCode's Swift projects.")
      opt[Boolean]("objc-gen-protocol").valueName("<true/false>").foreach(x => objcGenProtocol = x)
        .text("Generate Objective-C protocols for native (+c) only interfaces.")
      opt[Boolean]("objc-disable-class-ctor").valueName("<true/false>").foreach(x => objcDisableClassCtor = x)
        .text("Disable generating Objective-C class init helper method.")
      opt[Boolean]("objc-closed-enums").valueName("<true/false>").foreach(x => objcClosedEnums = x)
        .text("All generated Objective-C enums will be NS_CLOSED_ENUM (default: false). ")
      opt[Boolean]("objc-strict-protocols")
        .valueName("<true/false>").foreach(x => objcStrictProtocol = x)
        .text("All generated @protocol will implement <NSObject> (default: true). ")
      opt[Boolean]("objc-legacy-records")
        .valueName("<true/false>").foreach(x => objcLegacyRecords = x)
        .text("Use legacy record behavior for ObjC code (default: false)")
      note("")
      opt[Boolean]("objc-omit-full-convenience-constructor")
        .valueName("<omit-full-constructor>").foreach(x => objcOmitFullConvenienceConstructor = x)
        .text("Skips generation of the convenience constructor requiring all record parameters, if possible (default: false)")
      opt[File]("objcpp-out").valueName("<out-folder>").foreach(x => objcppOutFolder = Some(x))
        .text("The output folder for private Objective-C++ files (Generator disabled if unspecified).")
      opt[String]("objcpp-ext").valueName("<ext>").foreach(objcppExt = _)
        .text("The filename extension for Objective-C++ files (default: \"mm\")")
      opt[String]("objcpp-include-prefix").valueName("<prefix>").foreach(objcppIncludePrefix = _)
        .text("The prefix for #import of Objective-C++ header files from Objective-C++ files.")
      opt[String]("objcpp-include-cpp-prefix").valueName("<prefix>").foreach(objcppIncludeCppPrefix = _)
        .text("The prefix for #include of the main C++ header files from Objective-C++ files.")
      opt[String]("objcpp-include-objc-prefix").valueName("<prefix>").foreach(x => objcppIncludeObjcPrefixOptional = Some(x))
        .text("The prefix for #import of the Objective-C header files from Objective-C++ files (default: the same as --objcpp-include-prefix)")
      opt[String]("cpp-extended-record-include-prefix").valueName("<prefix>").foreach(cppExtendedRecordIncludePrefix = _)
        .text("The prefix path for #include of the extended record C++ header (.hpp) files")
      opt[String]("objc-extended-record-include-prefix").valueName("<prefix>").foreach(objcExtendedRecordIncludePrefix = _)
        .text("The prefix path for #import of the extended record Objective-C header (.h) files")
      opt[String]("objcpp-namespace").valueName("<prefix>").foreach(objcppNamespace = _)
        .text("The namespace name to use for generated Objective-C++ classes.")
      opt[String]("objcpp-function-prologue-file").valueName("<header-file>").foreach(x => objcppFunctionPrologueFile = Some(x))
        .text("User header file to include in generated Objective-C++ classes.")
      opt[Boolean]("objcpp-disable-exception-translation").valueName("<true/false>").foreach(x => objcppDisableExceptionTranslation = x)
        .text("Disable generating C++ -> Objective-C exception translation")
      opt[String]("objc-base-lib-include-prefix").valueName("...").foreach(x => objcBaseLibIncludePrefix = x)
        .text("The Objective-C++ base library's include path, relative to the Objective-C++ classes.")
      note("")
      opt[File]("wasm-out").valueName("<out-folder>").foreach(x => wasmOutFolder = Some(x))
        .text("The output for the WASM bridge C++ files (Generator disabled if unspecified).")
      opt[String]("wasm-include-prefix").valueName("<prefix>").foreach(wasmIncludePrefix = _)
        .text("The prefix for #includes of WASM header files from WASM C++ files.")
      opt[String]("wasm-include-cpp-prefix").valueName("<prefix>").foreach(wasmIncludeCppPrefix = _)
        .text("The prefix for #includes of the main header files from WASM C++ files.")
      opt[String]("wasm-base-lib-include-prefix").valueName("...").foreach(x => wasmBaseLibIncludePrefix = x)
        .text("The WASM base library's include path, relative to the WASM C++ classes.")
      opt[Boolean]("wasm-omit-constants").valueName("<true/false>").foreach(x => wasmOmitConstants = x)
        .text("Omit the generation of consts and enums in wasm, making them only accessible through TypeScript.")
      opt[String]("wasm-namespace").valueName("...").foreach(x => wasmNamespace = Some(x))
        .text("The namespace to use for generated Wasm classes.")
      opt[String]("wasm-cpp-namespace").valueName("...").foreach(x => wasmCppNamespace = Some(x))
        .text("The C++ namespace for generated WASM bridge classes (default: --jni-namespace).")
      opt[Boolean]("wasm-omit-namespace-alias").valueName("<true/false>").foreach(x => wasmOmitNsAlias = x)
        .text("Omit the generation of namespace aliases for classes. Namespaces will be prepended to class names instead.")
      opt[File]("ts-out").valueName("<out-folder>").foreach(x => tsOutFolder = Some(x))
        .text("The output for the TypeScript interface files (Generator disabled if unspecified).")
      opt[String]("ts-module").valueName("<name>").foreach(tsModule = _)
        .text("TypeScript declaration module name (default: \"module\").")
      note("")
      opt[File]("valdi-out").valueName("<out-folder>").foreach(x => valdiOutFolder = Some(x))
        .text("The output for the Valdi bridge C++ files (Generator disabled if unspecified).")
      opt[String]("valdi-include-prefix").valueName("<prefix>").foreach(valdiIncludePrefix = _)
        .text("The prefix for #includes of Valdi header files from Valdi C++ files.")
      opt[String]("valdi-include-cpp-prefix").valueName("<prefix>").foreach(valdiIncludeCppPrefix = _)
        .text("The prefix for #includes of the main header files from Valdi C++ files.")
      opt[String]("valdi-base-lib-include-prefix").valueName("...").foreach(x => valdiBaseLibIncludePrefix = x)
        .text("The Valdi base library's include path, relative to the Valdi C++ classes.")
      opt[String]("valdi-namespace").valueName("...").foreach(x => valdiNamespace = x)
        .text("The namespace name to use for generated Valdi C++ classes.")
      opt[File]("valdi-ts-out").valueName("<out-folder>").foreach(x => valdiTsOutFolder = Some(x))
        .text("The output for the Valdi TypeScript interface files (Generator disabled if unspecified).")
      note("")
      opt[File]("c-out").valueName("<out-folder>").foreach(x => cOutFolder = Some(x))
        .text("The output for the C files (Generated disabled if unspecified).")
      opt[String]("c-namespace").valueName("<namespace.").foreach(x => cNamespace = x)
        .text("The C namespace, used as function prefixes for generated functions")
      opt[File]("c-header-out").valueName("<out-folder>").foreach(x => cHeaderOutFolderOptional = Some(x))
        .text("The output folder for C header files (default: the same as --c-out).")
      opt[String]("c-base-lib-include-prefix").valueName("...").foreach(x => cBaseLibIncludePrefix = x)
        .text("The C base library's include path, relative to the C files.")
      opt[String]("c-include-prefix").valueName("...").foreach(x => cIncludePrefix = x)
        .text("The prefix for #includes of header files from C++ files.")
      opt[String]("c-wrapper-cpp-namespace").valueName("...").foreach(x => cWrapperCppNamespace = Some(x))
        .text("The C++ namespace to use for C API wrapper convenience classes.")
      opt[Boolean]( "c-wrapper-use-dlsym").valueName("<true/false>").foreach(x => cWrapperUseDlsym = x)
        .text("In C wrapper, resolve functions at runtime using dlsym() instead of directly linking at build-time. (default: false)")
      opt[File]("swift-out").valueName("<out-folder>").foreach(x => swiftOutFolder = Some(x))
        .text("The output folder for Swift files (Generator disabled if unspecified).")
      opt[String]("swift-module").valueName("<name>").foreach(swiftModule = _)
        .text("Swift module name (default: \"Module\").")
      opt[Unit]("swift-private-in-same-module").foreach(_ => swiftPrivateInSameModule = true)
        .text("Omit 'import <module>' in +Private Swift files (use when those files are compiled in the same module to avoid 'ignoring import' warnings).")
      opt[File]("swiftxx-out").valueName("<out-folder>").foreach(x => swiftxxOutFolder = Some(x))
        .text("The output folder for private Swift/C++ interop files (Generator disabled if unspecified).")
      opt[String]("swiftxx-include-prefix").valueName("<prefix>").foreach(swiftxxIncludePrefix = _)
        .text("The prefix for #includes of Swift C++ header files.")
      opt[String]("swiftxx-include-cpp-prefix").valueName("<prefix>").foreach(swiftxxIncludeCppPrefix = _)
        .text("The prefix for #includes of the main header files from Swift C++ files.")
      opt[String]("swiftxx-base-lib-include-prefix").valueName("...").foreach(x => swiftxxBaseLibIncludePrefix = x)
        .text("The Swift C++ base library's include path, relative to the Swift C++ classes.")
      note("")
      opt[File]("yaml-out").valueName("<out-folder>").foreach(x => yamlOutFolder = Some(x))
        .text("The output folder for YAML files (Generator disabled if unspecified).")
      opt[String]("yaml-out-file").valueName("<out-file>").foreach(x => yamlOutFile = Some(x))
        .text("If specified all types are merged into a single YAML file instead of generating one file per type (relative to --yaml-out).")
      opt[String]("yaml-prefix").valueName("<pre>").foreach(yamlPrefix = _)
        .text("The prefix to add to type names stored in YAML files (default: \"\").")
      note("")
      opt[File]("list-in-files").valueName("<list-in-files>").foreach(x => inFileListPath = Some(x))
        .text("Optional file in which to write the list of input files parsed.")
      opt[File]("list-out-files").valueName("<list-out-files>").foreach(x => outFileListPath = Some(x))
        .text("Optional file in which to write the list of output files produced.")
      opt[File]("list-declarations").valueName("<list-declarations>").foreach(x => declarationManifestPath = Some(x))
        .text("Write internal type names, kinds, and generation-relevant properties as TSV.")
      opt[File]("verify-declarations").valueName("<verify-declarations>").foreach(x => verifyDeclarationManifestPath = Some(x))
        .text("Verify the resolved declarations against a manifest.")
      opt[Boolean]("skip-generation").valueName("<true/false>").foreach(x => skipGeneration = x)
        .text("Way of specifying if file generation should be skipped (default: false)")

      note("\nIdentifier styles (ex: \"FooBar\", \"fooBar\", \"foo_bar\", \"FOO_BAR\", \"m_fooBar\")\nUse an exclamation mark to apply stricty, even on ALL_CAPS identifiers (ex: \"FooBar!\")\n")
      identStyle("ident-java-enum",      c => { javaIdentStyle = javaIdentStyle.copy(enum = c) })
      identStyle("ident-java-field",     c => { javaIdentStyle = javaIdentStyle.copy(field = c) })
      identStyle("ident-java-type",      c => { javaIdentStyle = javaIdentStyle.copy(ty = c) })
      identStyle("ident-cpp-enum",       c => { cppIdentStyle = cppIdentStyle.copy(enum = c) })
      identStyle("ident-cpp-field",      c => { cppIdentStyle = cppIdentStyle.copy(field = c) })
      identStyle("ident-cpp-method",     c => { cppIdentStyle = cppIdentStyle.copy(method = c) })
      identStyle("ident-cpp-type",       c => { cppIdentStyle = cppIdentStyle.copy(ty = c) })
      identStyle("ident-cpp-enum-type",  c => { cppTypeEnumIdentStyle = c })
      identStyle("ident-cpp-type-param", c => { cppIdentStyle = cppIdentStyle.copy(typeParam = c) })
      identStyle("ident-cpp-local",      c => { cppIdentStyle = cppIdentStyle.copy(local = c) })
      identStyle("ident-cpp-file",       c => { cppFileIdentStyle = c })
      identStyle("ident-jni-class",      c => { jniClassIdentStyleOptional = Some(c)})
      identStyle("ident-jni-file",       c => { jniFileIdentStyleOptional = Some(c)})
      identStyle("ident-wasm-class",     c => { wasmClassIdentStyleOptional = Some(c)})
      identStyle("ident-wasm-file",      c => { wasmFileIdentStyleOptional = Some(c)})
      identStyle("ident-objc-enum",       c => { objcIdentStyle = objcIdentStyle.copy(enum = c) })
      identStyle("ident-objc-field",      c => { objcIdentStyle = objcIdentStyle.copy(field = c) })
      identStyle("ident-objc-method",     c => { objcIdentStyle = objcIdentStyle.copy(method = c) })
      identStyle("ident-objc-type",       c => { objcIdentStyle = objcIdentStyle.copy(ty = c) })
      identStyle("ident-objc-type-param", c => { objcIdentStyle = objcIdentStyle.copy(typeParam = c) })
      identStyle("ident-objc-local",      c => { objcIdentStyle = objcIdentStyle.copy(local = c) })
      identStyle("ident-objc-const",      c => { objcIdentStyle = objcIdentStyle.copy(const = c) })
      identStyle("ident-objc-file",       c => { objcFileIdentStyleOptional = Some(c) })
      identStyle("ident-valdi-class", c => { valdiClassIdentStyleOptional = Some(c)})
      identStyle("ident-valdi-file",  c => { valdiFileIdentStyleOptional = Some(c)})
      identStyle("ident-swiftxx-class", c => { swiftxxClassIdentStyleOptional = Some(c)})
      identStyle("ident-swiftxx-file",  c => { swiftxxFileIdentStyleOptional = Some(c)})
    }

    if (!argParser.parse(parseArgs)) {
      System.exit(1); return
    }

    val cppHeaderOutFolder = if (cppHeaderOutFolderOptional.isDefined) cppHeaderOutFolderOptional else cppOutFolder
    val cHeaderOutFolder = if (cHeaderOutFolderOptional.isDefined) cHeaderOutFolderOptional else cOutFolder
    val jniHeaderOutFolder = if (jniHeaderOutFolderOptional.isDefined) jniHeaderOutFolderOptional else jniOutFolder
    val jniClassIdentStyle = jniClassIdentStyleOptional.getOrElse(cppIdentStyle.ty)
    val jniBaseLibClassIdentStyle = jniBaseLibClassIdentStyleOptional.getOrElse(jniClassIdentStyle)
    val jniFileIdentStyle = jniFileIdentStyleOptional.getOrElse(cppFileIdentStyle)
    var objcFileIdentStyle = objcFileIdentStyleOptional.getOrElse(objcIdentStyle.ty)
    val objcppIncludeObjcPrefix = objcppIncludeObjcPrefixOptional.getOrElse(objcppIncludePrefix)

    // Add ObjC prefix to identstyle
    objcIdentStyle = objcIdentStyle.copy(ty = IdentStyle.prefix(objcTypePrefix,objcIdentStyle.ty))
    objcFileIdentStyle = IdentStyle.prefix(objcTypePrefix, objcFileIdentStyle)

    val valdiClassIdentStyle = valdiClassIdentStyleOptional.getOrElse(cppIdentStyle.ty)
    val valdiFileIdentStyle = valdiFileIdentStyleOptional.getOrElse(cppFileIdentStyle)

    if (cppTypeEnumIdentStyle != null) {
      cppIdentStyle = cppIdentStyle.copy(enumType = cppTypeEnumIdentStyle)
    }

    val swiftxxClassIdentStyle = swiftxxClassIdentStyleOptional.getOrElse(cppIdentStyle.ty)
    val swiftxxFileIdentStyle = swiftxxFileIdentStyleOptional.getOrElse(cppFileIdentStyle)

    // Parse IDL file.
    System.out.println("Parsing...")
    val inFileListWriter = if (inFileListPath.isDefined) {
      if (inFileListPath.get.getParentFile != null)
        createFolder("input file list", inFileListPath.get.getParentFile)
      Some(new BufferedWriter(new FileWriter(inFileListPath.get)))
    } else {
      None
    }
    val parser = new Parser(idlIncludePaths)
    val (idl, flags) = try {
      parser.parseFile(idlFile, inFileListWriter)
    }
    catch {
      case ex @ (_: FileNotFoundException | _: IOException) =>
        System.err.println("Error reading from --idl file: " + ex.getMessage)
        System.exit(1); return
    }
    finally {
      if (inFileListWriter.isDefined) {
        inFileListWriter.get.close()
      }
    }
    if (!flags.isEmpty) {
      if (!argParser.parse(flags.map({_.split(" ", 2)}).flatten :+ "--idl" :+ idlFile.getName())) {
        System.exit(1); return
      }
    }

    if (declarationManifestPath.isDefined || verifyDeclarationManifestPath.isDefined) {
      val declarations = idl.collect { case td: ast.InternTypeDecl => td }.map { td =>
        val (kind, hasConstants, hasDerivings, extensions) = td.body match {
          case e: ast.Enum => (if (e.flags) "flags" else "enum", false, false, "")
          case r: ast.Record => ("record", r.consts.nonEmpty, r.derivingTypes.nonEmpty, Seq(r.ext.java, r.ext.cpp, r.ext.cc, r.ext.objc, r.ext.js, r.ext.swift).mkString(","))
          case i: ast.Interface => ("interface", i.consts.nonEmpty, false, Seq(i.ext.java, i.ext.cpp, i.ext.cc, i.ext.objc, i.ext.js, i.ext.swift).mkString(","))
          case _: ast.ProtobufMessage => ("protobuf", false, false, "")
        }
        Seq(td.ident.name, kind, td.params.length.toString, hasConstants.toString, hasDerivings.toString, extensions).mkString("\t")
      }.mkString("", "\n", "\n")
      verifyDeclarationManifestPath.foreach { path =>
        val existing = scala.io.Source.fromFile(path, "UTF-8")
        try {
          if (existing.mkString != declarations) {
            System.err.println("Declaration manifest does not match resolved IDL: " + path)
            System.exit(1)
          }
        } finally existing.close()
      }
      declarationManifestPath.foreach { path =>
        if (path.getParentFile != null) createFolder("declaration manifest", path.getParentFile)
        val writer = new BufferedWriter(new FileWriter(path))
        try writer.write(declarations) finally writer.close()
      }
    }

    System.out.println("Generating...")
    val outFileListWriter = if (outFileListPath.isDefined) {
      if (outFileListPath.get.getParentFile != null)
        createFolder("output file list", outFileListPath.get.getParentFile)
      Some(new BufferedWriter(new FileWriter(outFileListPath.get)))
    } else {
      None
    }
    val objcSwiftBridgingHeaderWriter = if (objcSwiftBridgingHeaderName.isDefined && objcOutFolder.isDefined) {
      val objcSwiftBridgingHeaderFile = new File(objcOutFolder.get.getPath, objcSwiftBridgingHeaderName.get + ".h")
      if (objcSwiftBridgingHeaderFile.getParentFile != null)
        createFolder("output file list", objcSwiftBridgingHeaderFile.getParentFile)
      Some(new BufferedWriter(new FileWriter(objcSwiftBridgingHeaderFile)))
    } else {
      None
    }

    val outSpec = Spec(
      javaOutFolder,
      javaPackage,
      javaClassAccessModifier,
      javaIdentStyle,
      javaCppException,
      javaAnnotation,
      javaNullableAnnotation,
      javaNonnullAnnotation,
      javaImplementAndroidOsParcelable,
      javaUseFinalForRecord,
      javaGenInterface,
      javaLegacyRecords,
      cppOutFolder,
      cppHeaderOutFolder,
      cppIncludePrefix,
      cppExtendedRecordIncludePrefix,
      cppNamespace,
      cppIdentStyle,
      cppFileIdentStyle,
      cppBaseLibIncludePrefix,
      cppOptionalTemplate,
      cppOptionalHeader,
      cppNulloptValue,
      cppEnumHashWorkaround,
      cppNnHeader,
      cppNnType,
      cppNnCheckExpression,
      cppUseWideStrings,
      cppLegacyRecords,
      jniOutFolder,
      jniHeaderOutFolder,
      jniIncludePrefix,
      jniIncludeCppPrefix,
      jniNamespace,
      jniClassIdentStyle,
      jniFileIdentStyle,
      jniBaseLibIncludePrefix,
      jniUseOnLoad,
      jniFunctionPrologueFile,
      cppExt,
      cppHeaderExt,
      objcOutFolder,
      objcppOutFolder,
      objcIdentStyle,
      objcFileIdentStyle,
      objcppExt,
      objcHeaderExt,
      objcIncludePrefix,
      objcExtendedRecordIncludePrefix,
      objcppIncludePrefix,
      objcppIncludeCppPrefix,
      objcppIncludeObjcPrefix,
      objcppNamespace,
      objcppFunctionPrologueFile,
      objcppDisableExceptionTranslation,
      objcBaseLibIncludePrefix,
      objcSwiftBridgingHeaderWriter,
      objcSwiftBridgingHeaderName,
      objcGenProtocol,
      objcDisableClassCtor,
      objcClosedEnums,
      objcStrictProtocol,
      objcLegacyRecords,
      objcOmitFullConvenienceConstructor,
      wasmOutFolder,
      wasmIncludePrefix,
      wasmIncludeCppPrefix,
      wasmBaseLibIncludePrefix,
      wasmOmitConstants,
      wasmNamespace,
      wasmOmitNsAlias,
      wasmCppNamespace.getOrElse(jniNamespace),
      wasmClassIdentStyleOptional.getOrElse(jniClassIdentStyle),
      wasmFileIdentStyleOptional.getOrElse(jniFileIdentStyle),
      jsIdentStyle,
      tsOutFolder,
      tsModule,
      valdiOutFolder,
      valdiIncludePrefix,
      valdiIncludeCppPrefix,
      valdiBaseLibIncludePrefix,
      valdiNamespace,
      valdiClassIdentStyle,
      valdiFileIdentStyle,
      valdiTsOutFolder,
      cOutFolder,
      cHeaderOutFolder,
      cNamespace,
      cBaseLibIncludePrefix,
      cIncludePrefix,
      cWrapperCppNamespace,
      cWrapperUseDlsym,
      swiftOutFolder,
      swiftIdentStyle,
      swiftModule,
      swiftPrivateInSameModule,
      swiftxxOutFolder,
      swiftxxNamespace,
      swiftxxIncludePrefix,
      swiftxxBaseLibModule,
      swiftxxClassIdentStyle,
      swiftxxFileIdentStyle,
      swiftxxIncludeCppPrefix,
      swiftxxBaseLibIncludePrefix,
      outFileListWriter,
      skipGeneration,
      yamlOutFolder,
      yamlOutFile,
      yamlPrefix,
      idlFile.getName.stripSuffix(".djinni"))

    resolver.resolve(meta.defaults, idl, outSpec) match {
      case Some(err) =>
        System.err.println(err)
        System.exit(1); return
      case _ =>
    }

    try {
      val r = generate(idl, outSpec)
      r.foreach { e =>
        System.err.println("Error generating output: " + e)
        System.exit(1)
      }
    }
    finally {
      if (outFileListWriter.isDefined) {
        outFileListWriter.get.close()
      }
      if (objcSwiftBridgingHeaderWriter.isDefined) {
        objcSwiftBridgingHeaderWriter.get.close()
      }
    }
  }
}
