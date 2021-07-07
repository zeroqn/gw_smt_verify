use gw_common::smt::Blake2bHasher;
use gw_common::sparse_merkle_tree::CompiledMerkleProof;
use gw_common::H256;
use molecule::prelude::*;

mod conversion;
use conversion::Unpack;
mod smt;
use smt::SMTCase;

fn main() {
    let data = include_bytes!("../../data");
    let case = SMTCase::from_slice(data).unwrap();

    let proof = CompiledMerkleProof(case.proof().unpack().to_vec());
    let kv_state: Vec<(H256, H256)> = case.kv_state().unpack();
    let root = proof.compute_root::<Blake2bHasher>(kv_state).unwrap();
    assert_eq!(root, case.root().unpack());

    println!("successful verify merkle root")
}
