//! MPEG-1/2 video decoder imported from siglus_rs. See UPSTREAM.md.
pub mod convert;
pub mod demux;
pub mod pipeline;
pub mod video;

pub use convert::{frame_to_gray_rgba, frame_to_rgba_bt601_limited, frame_to_rgba_bt601_limited_scaled};
pub use demux::{Demuxer, Packet, StreamType};
pub use pipeline::MpegVideoPipeline;
pub use video::{Decoder, Frame, PixelFormat};
