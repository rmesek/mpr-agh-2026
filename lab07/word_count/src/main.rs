use ahash::AHashMap;
use memmap2::MmapOptions;
use std::env;
use std::fs::File;
use std::io;

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <file>", args[0]);
        std::process::exit(1);
    }

    let file = File::open(&args[1])?;
    let mmap = unsafe { MmapOptions::new().map(&file)? };

    let mut word_counts: AHashMap<&[u8], u64> = AHashMap::default();

    for word in mmap.split(|b| b.is_ascii_whitespace()) {
        if !word.is_empty() {
            *word_counts.entry(word).or_insert(0) += 1;
        }
    }

    println!("Unique words: {}", word_counts.len());
    Ok(())
}
