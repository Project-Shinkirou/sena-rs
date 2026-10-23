//! Compile with rustc, then redirect stdout to reverse/EXTCALL_CATALOG.md.
//! Uses effective registry lookup so automatic entries cannot double-count
//! explicit overrides. Status is evidence metadata, not runtime coverage.

#[allow(dead_code)]
#[path = "../../crates/pal-script/src/extsig.rs"]
mod extsig;

use std::collections::{BTreeMap, BTreeSet};

fn cell(value: &str) -> String {
    value.replace('|', "\\|").replace(['\n', '\r'], " ")
}

fn main() {
    let explicit: BTreeSet<_> = extsig::all_signatures()
        .iter()
        .map(|sig| (sig.category, sig.index))
        .collect();
    let automatic: BTreeSet<_> = extsig::auto_signatures()
        .iter()
        .map(|sig| (sig.category, sig.index))
        .collect();
    let keys: BTreeSet<_> = explicit.union(&automatic).copied().collect();
    let mut counts = BTreeMap::<String, usize>::new();
    for &(category, index) in &keys {
        let sig = extsig::lookup_sig(category, index).unwrap();
        *counts
            .entry(format!("{:?}", sig.implementation_status))
            .or_default() += 1;
    }
    println!("# Effective extcall signature catalog\n");
    println!("Generated from the shared Rust registry by `reverse/evidence/extcall_catalog.rs`.\n");
    println!("{} explicit coordinates and {} automatic coordinates overlap at {}; the effective union contains **{} unique coordinates**.\n",
        explicit.len(), automatic.len(), explicit.intersection(&automatic).count(), keys.len());
    println!("This is a registry inventory, not a complete native interface table. `Blocked` means insufficient verification, not an absent handler. `Verified` does not certify every runtime boundary. Runtime corrections and remaining gaps are recorded in [port-corrections.md](port-corrections.md).\n");
    println!("| Recorded status | Count |\n|---|---:|");
    for (status, count) in counts {
        println!("| {status} | {count} |");
    }
    println!("\n| Coordinate | Name | Pop | Display parameters (pop index) | Status | Source | Purpose |\n|---|---|---:|---|---|---|---|");
    for (category, index) in keys {
        let sig = extsig::lookup_sig(category, index).unwrap();
        let params = sig
            .params
            .iter()
            .map(|p| format!("{} ({}, {:?})", p.name, p.pop_idx, p.kind))
            .collect::<Vec<_>>()
            .join(", ");
        println!(
            "| {category}:{index} | `{}` | {} | {} | {:?} | {} | {} |",
            cell(sig.name),
            sig.pop_count,
            cell(&params),
            sig.implementation_status,
            if explicit.contains(&(category, index)) {
                "explicit"
            } else {
                "automatic"
            },
            cell(sig.purpose)
        );
    }
}
