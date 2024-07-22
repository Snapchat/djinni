plugins {
    `java-library`
    `maven-publish`
}

repositories {
    mavenCentral()
}

// We have BUILD file from basel
getLayout().buildDirectory.set(file("gradle_build"))

sourceSets {
    main {
        java {
            srcDirs("java")
        }
    }
}

java {
    toolchain {
        languageVersion = JavaLanguageVersion.of(17)
    }
}

java {
    withJavadocJar()
    withSourcesJar()
}

publishing {
    publications {
        create<MavenPublication>("maven") {
            groupId = "com.bandlab.djinni"
            artifactId = "djinni-support-lib"
            version = project.providers.gradleProperty("version").orNull

            from(components["java"])
        }
    }

    repositories {
        maven(project.providers.gradleProperty("artifactoryPublishUrl")) {
            name = "artifactory"
            credentials(PasswordCredentials::class.java)
        }
    }
}
