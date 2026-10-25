#![cfg_attr(target_os = "vita", feature(alloc_error_hook))]
//! Console link root. All engine semantics live in pal-vm.
pub use pal_vm::ffi::*;

/// The C logger uses an unbuffered stream so an OOM report needs no Rust allocation.
#[no_mangle]
pub extern "C" fn sena_vita_install_alloc_reporter() {
    unsafe extern "C" {
        fn sena_vita_allocation_failed(size: usize, align: usize);
        fn sena_vita_panic_report(message: *const u8, len: usize,
            file: *const u8, file_len: usize, line: u32);
    }
    std::panic::set_hook(Box::new(|info| {
        let message = info.payload().downcast_ref::<&str>().copied()
            .or_else(|| info.payload().downcast_ref::<String>().map(String::as_str))
            .unwrap_or("panic with non-string payload");
        let (file, line) = info.location().map(|loc| (loc.file(), loc.line()))
            .unwrap_or(("unknown", 0));
        unsafe {
            sena_vita_panic_report(message.as_ptr(), message.len(),
                file.as_ptr(), file.len(), line);
        }
    }));
    std::alloc::set_alloc_error_hook(|layout| unsafe {
        sena_vita_allocation_failed(layout.size(), layout.align());
    });
}

#[cfg(target_os = "vita")]
mod cpu_allocator {
    use std::alloc::{GlobalAlloc, Layout, System};
    struct VitaAllocator;
    const LARGE: usize = 1024 * 1024;
    fn isolated(layout: Layout) -> bool { layout.size() >= LARGE && layout.align() <= 4096 }
    unsafe extern "C" {
        fn sena_vita_cpu_alloc(bytes: usize, zero: bool) -> *mut u8;
        fn sena_vita_cpu_free(ptr: *mut u8);
    }
    unsafe impl GlobalAlloc for VitaAllocator {
        unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
            if isolated(layout) { unsafe { sena_vita_cpu_alloc(layout.size(), false) } }
            else { unsafe { System.alloc(layout) } }
        }
        unsafe fn alloc_zeroed(&self, layout: Layout) -> *mut u8 {
            if isolated(layout) { unsafe { sena_vita_cpu_alloc(layout.size(), true) } }
            else { unsafe { System.alloc_zeroed(layout) } }
        }
        unsafe fn dealloc(&self, ptr: *mut u8, layout: Layout) {
            if isolated(layout) { unsafe { sena_vita_cpu_free(ptr) } }
            else { unsafe { System.dealloc(ptr, layout) } }
        }
        unsafe fn realloc(&self, ptr: *mut u8, old: Layout, size: usize) -> *mut u8 {
            let Ok(next) = Layout::from_size_align(size, old.align()) else { return std::ptr::null_mut(); };
            if !isolated(old) && !isolated(next) { return unsafe { System.realloc(ptr, old, size) }; }
            let output = unsafe { self.alloc(next) };
            if !output.is_null() {
                unsafe {
                    std::ptr::copy_nonoverlapping(ptr, output, old.size().min(size));
                    self.dealloc(ptr, old);
                }
            }
            output
        }
    }
    #[global_allocator]
    static ALLOCATOR: VitaAllocator = VitaAllocator;
}
