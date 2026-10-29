# Djinni Generation Configuration

This companion configuration format is modeled from the CLI options
used by `examples/run_djinni.sh`, `perftest/run_djinni.sh`,
`test-suite/run_djinni.sh`, and `external-test/cli_equivalence_test.sh`. The
companion YAML sits beside its root `.djinni` file.

```yaml
schema_version: 1

languages:
  cpp:
    namespace: example
    optional_template: std::optional
    optional_header: <optional>
    identifiers:
      enum_type: foo_bar
  java:
    package: com.example
    use_final_for_record: false
  objc:
    type_prefix: EX
  objcpp: {}
```

Language sections contain customization options only. The generation rule
selects backends and output directories. Identifier overrides are nested under
their backend. CLI names map to snake case under their backend: for example,
`--cpp-namespace` becomes
`languages.cpp.namespace`, and `--ident-jni-class` becomes
`languages.jni.identifiers.class`. Boolean CLI options become YAML booleans.

Shared parser inputs such as IDL include paths stay with the Bazel target rather
than this file. Operational options such as `--idl`, output directories, and
input/output file lists also stay with the invocation.

`yaml` records the existing `--yaml-out-file` and `--yaml-prefix`
settings used by the YAML round-trip tests. It is specific to those test
profiles and is not a required language section.

The compiler reads a configuration only when given `--config <file>`; the file
is never discovered implicitly. The `--verify-config` mode checks that language
customization flags on the CLI exactly match the YAML. The repository's
generation scripts use that mode for side-by-side comparison:

| Profile | Invocation |
| --- | --- |
| `examples/example.djinni.yaml` | `examples/run_djinni.sh` |
| `perftest/djinni_perf_benchmark.djinni.yaml` | `perftest/run_djinni.sh` |
| `external-test/schema.djinni.yaml` | `external-test/cli_equivalence_test.sh` |
| `test-suite/djinni/wchar_test.djinni.yaml` | wchar fixture generation |
| `test-suite/djinni/all.djinni.yaml` | full test-suite generation |
| `test-suite/djinni/function_prologue.djinni.yaml` | prologue fixture generation |
| `test-suite/djinni/ident_explicit.djinni.yaml` | explicit identifier fixture generation |
| `test-suite/djinni/optionals.djinni.yaml` | optionals fixture generation |
| `test-suite/djinni/interface_and_abstract_class.djinni.yaml` | interface fixture generation |
| `test-suite/djinni/yaml-test.djinni.yaml` | YAML round-trip generation |

Each profile belongs to a root IDL invocation; settings currently apply to all
declarations in that invocation, including its `@import` closure. The YAML
round-trip command actually reads a temporary copy of `yaml-test.djinni`; its
profile is kept beside the source fixture as the model for that invocation.

Bazel targets also invoke Djinni through rule attributes. Those settings are
not duplicated in this direct-CLI inventory, and some differ from the shell
scripts (for example, the test-suite Bazel target does not set wide strings).
Those differences need to be reconciled before the config becomes
authoritative. `--list-in-files` and `--list-out-files` are omitted because they
describe input tracking and generated artifacts, not backend customization.
