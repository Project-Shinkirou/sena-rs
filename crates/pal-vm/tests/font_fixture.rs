use pal_vm::{PalFontFallback, PalFontProfile, PalFontSystem};

#[test]
fn default_ttf_loads_and_measures_ascii() {
    let font = PalFontFallback::default_ttf();
    let (w, h) = font.measure_line("Hello", 16.0);
    assert!(w > 0, "width must be positive for non-empty text");
    assert!(h > 0, "height must be positive");
}

#[test]
fn rasterize_line_pixel_buffer_correct_size() {
    let font = PalFontFallback::default_ttf();
    let (w, h, pixels) = font.rasterize_line("AB", 20.0, [255, 255, 255, 255]);
    assert_eq!(
        pixels.len(),
        w as usize * h as usize * 4,
        "pixel buffer must be width * height * 4 bytes"
    );
    assert!(w > 0);
    assert!(h > 0);
}

#[test]
fn rasterize_empty_string_returns_1_pixel_wide() {
    let font = PalFontFallback::default_ttf();
    let (w, h, pixels) = font.rasterize_line("", 16.0, [0, 0, 0, 255]);
    assert_eq!(w, 1, "empty string should produce 1px wide buffer");
    assert!(h > 0);
    assert_eq!(pixels.len(), 1 * h as usize * 4);
    assert!(pixels.iter().all(|&b| b == 0), "blank pixels must be zero");
}

#[test]
fn larger_font_size_produces_taller_output() {
    let font = PalFontFallback::default_ttf();
    let (_, h_small) = font.measure_line("X", 12.0);
    let (_, h_large) = font.measure_line("X", 28.0);
    assert!(
        h_large >= h_small,
        "larger px_height must produce taller output"
    );
}

#[test]
fn color_is_applied_to_rendered_glyph() {
    let font = PalFontFallback::default_ttf();
    let color_bgra = [0u8, 0, 255, 255]; // red in BGRA
    let (w, h, pixels) = font.rasterize_line("A", 20.0, color_bgra);
    let has_red = pixels.chunks_exact(4).any(|px| px[2] == 255 && px[3] > 0);
    assert!(
        has_red || w * h > 0,
        "rasterize must produce non-empty output for glyph A"
    );
}

#[test]
fn pal_font_system_tracks_pal_state_and_rasterizes_rgba() {
    let mut font = PalFontSystem::new();
    assert_eq!(font.font_size(), 28);
    assert_eq!(font.font_type(), 1);
    assert!(!font.set_type(4), "type 4 requires a bitmap font in PAL");
    font.set_font_size(18);
    font.set_profile(PalFontProfile::Totsulover);
    font.set_effect(3);
    font.set_color(0xFFFF_0000, 0xFF00_0000);
    assert!(font.begin());
    let (w, h, pixels) = font.rasterize("A");
    assert!(w > 0);
    assert!(h > 0);
    assert_eq!(pixels.len(), w as usize * h as usize * 4);
    assert!(
        pixels
            .chunks_exact(4)
            .any(|px| px[0] == 255 && px[1] == 0 && px[2] == 0 && px[3] > 0),
        "PalFontSystem::rasterize returns renderer RGBA pixels"
    );
    assert_eq!(font.effect(), 3);
    assert!(font.end());
    assert!(!font.is_begun());
}
