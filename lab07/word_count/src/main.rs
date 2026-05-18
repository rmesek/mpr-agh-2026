use ahash::AHashMap;
use encoding_rs::WINDOWS_1252;
use memmap2::MmapOptions;
use std::env;
use std::fs::File;
use std::io::{self, BufWriter, Write};

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <file>", args[0]);
        std::process::exit(1);
    }

    let file = File::open(&args[1])?;
    let mmap = unsafe { MmapOptions::new().map(&file)? };

    let mut word_counts: AHashMap<&[u8], u64> = AHashMap::default();

    // Splitting by any ASCII whitespace
    for word in mmap.split(|b| b.is_ascii_whitespace()) {
        if !word.is_empty() {
            *word_counts.entry(word).or_insert(0) += 1;
        }
    }

    // Fast buffered output
    let stdout = io::stdout();
    let mut handle = BufWriter::new(stdout.lock());

    for (word_bytes, count) in word_counts {
        // Decode the bytes using the Windows-1252 encoding
        let (word_str, _encoding_used, _had_errors) = WINDOWS_1252.decode(word_bytes);

        writeln!(handle, "{}\t{}", word_str, count)?;
    }

    Ok(())
}
