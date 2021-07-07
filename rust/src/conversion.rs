use crate::smt::{
    self, Byte32, Byte32Reader, BytesReader, KVPair, KVPairReader, KVPairVec, KVPairVecReader,
};
use bytes::Bytes;
use molecule::prelude::*;
use sparse_merkle_tree::H256;

pub trait Unpack<T> {
    fn unpack(&self) -> T;
}

macro_rules! impl_conversion_for_entity_unpack {
    ($original:ty, $entity:ident) => {
        impl Unpack<$original> for $entity {
            fn unpack(&self) -> $original {
                self.as_reader().unpack()
            }
        }
    };
}

macro_rules! impl_conversion_for_vector_unpack {
    ($original:ty, $entity:ident, $reader:ident) => {
        impl<'r> Unpack<Vec<$original>> for $reader<'r> {
            fn unpack(&self) -> Vec<$original> {
                self.iter().map(|x| x.unpack()).collect()
            }
        }
        impl_conversion_for_entity_unpack!(Vec<$original>, $entity);
    };
}

macro_rules! impl_conversion_for_vector {
    ($original:ty, $entity:ident, $reader:ident) => {
        impl_conversion_for_vector_unpack!($original, $entity, $reader);
    };
}

impl<'r> Unpack<(H256, H256)> for KVPairReader<'r> {
    fn unpack(&self) -> (H256, H256) {
        (self.k().unpack(), self.v().unpack())
    }
}

impl<'r> Unpack<Bytes> for BytesReader<'r> {
    fn unpack(&self) -> Bytes {
        Bytes::from(self.raw_data().to_vec())
    }
}

impl<'r> Unpack<H256> for Byte32Reader<'r> {
    fn unpack(&self) -> H256 {
        let ptr = self.as_slice().as_ptr() as *const [u8; 32];
        let r = unsafe { *ptr };
        r.into()
    }
}

impl Unpack<Bytes> for smt::Bytes {
    fn unpack(&self) -> Bytes {
        Bytes::from(self.raw_data().to_vec())
    }
}

impl_conversion_for_entity_unpack!((H256, H256), KVPair);
impl_conversion_for_vector!((H256, H256), KVPairVec, KVPairVecReader);
impl_conversion_for_entity_unpack!(H256, Byte32);
