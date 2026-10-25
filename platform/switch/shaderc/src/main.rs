//! Adapted from siglus_rs platform/switch/shaderc (5f358ef), MPL-2.0.
//! Compile the same WGSL used by desktop to GLSL input for devkitPro uam.
use naga::back::glsl;
use std::path::PathBuf;
fn main() {
    let args: Vec<_> = std::env::args_os().skip(1).collect();
    assert_eq!(
        args.len(),
        2,
        "usage: sena-switch-shaderc <WGSL directory> <output directory>"
    );
    let source_dir = PathBuf::from(&args[0]);
    let output_dir = PathBuf::from(&args[1]);
    std::fs::create_dir_all(&output_dir).unwrap();
    for (file, name) in [
        ("draw_primitive_up_2d.wgsl", "sprite"),
        ("raster.wgsl", "raster"),
    ] {
        let source = std::fs::read_to_string(source_dir.join(file)).unwrap();
        let module = naga::front::wgsl::parse_str(&source)
            .unwrap_or_else(|e| panic!("{}", e.emit_to_string(&source)));
        let info = naga::valid::Validator::new(
            naga::valid::ValidationFlags::all(),
            naga::valid::Capabilities::all(),
        )
        .validate(&module)
        .unwrap();
        for (entry, stage, suffix) in [
            ("vs_main", naga::ShaderStage::Vertex, "vsh"),
            ("fs_main", naga::ShaderStage::Fragment, "fsh"),
        ] {
            let mut bindings = glsl::BindingMap::default();
            bindings.insert(
                naga::ResourceBinding {
                    group: 0,
                    binding: 0,
                },
                0,
            );
            bindings.insert(
                naga::ResourceBinding {
                    group: 0,
                    binding: 2,
                },
                0,
            );
            bindings.insert(
                naga::ResourceBinding {
                    group: 1,
                    binding: 0,
                },
                0,
            );
            let options = glsl::Options {
                version: glsl::Version::Desktop(460),
                writer_flags: glsl::WriterFlags::empty(),
                binding_map: bindings,
                zero_initialize_workgroup_memory: false,
            };
            let pipeline = glsl::PipelineOptions {
                shader_stage: stage,
                entry_point: entry.to_owned(),
                multiview: None,
            };
            let mut output = String::new();
            glsl::Writer::new(
                &mut output,
                &module,
                &info,
                &options,
                &pipeline,
                naga::proc::BoundsCheckPolicies::default(),
            )
            .unwrap()
            .write()
            .unwrap();
            std::fs::write(output_dir.join(format!("{name}_{suffix}.glsl")), output).unwrap();
        }
    }
}
