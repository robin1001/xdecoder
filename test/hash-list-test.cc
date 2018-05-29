// util/hash-list-test.cc

// Copyright 2009-2011     Microsoft Corporation
//                2013     Johns Hopkins University (author: Daniel Povey)

// See ../../COPYING for clarification regarding multiple authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <time.h>

#include <map>  // for baseline.
#include <iostream>

#include "hash-list.h"

namespace xdecoder {

template<class Int, class T> void TestHashList() {
  typedef typename HashList<Int, T>::Elem Elem;
  srand(time(NULL));
  HashList<Int, T> hash;
  hash.SetSize(200);  // must be called before use.
  std::map<Int, T> m1;
  for (size_t j = 0; j < 50; j++) {
    Int key = rand() % 200;
    T val = rand() % 50;
    m1[key] = val;
    Elem *e = hash.Find(key);
    if (e) e->val = val;
    else  hash.Insert(key, val);
  }


  std::map<Int, T> m2;

  for (int i = 0; i < 100; i++) {
    m2.clear();
    for (typename std::map<Int, T>::const_iterator iter = m1.begin();
        iter != m1.end();
        iter++) {
      m2[iter->first + 1] = iter->second;
    }
    std::swap(m1, m2);

    Elem *h = hash.Clear(), *tmp;

    hash.SetSize(100 + rand() % 100);  // note, SetSize is relatively cheap
    // operation as long as we are not increasing the size more than it's ever
    // previously been increased to.

    for (; h != NULL; h = tmp) {
      hash.Insert(h->key + 1, h->val);
      tmp = h->tail;
      hash.Delete(h);  // think of this like calling delete.
    }

    // Now make sure h and m2 are the same.
    const Elem *list = hash.GetList();
    size_t count = 0;
    for (; list != NULL; list = list->tail, count++) {
      CHECK(m1[list->key] == list->val);
    }

    for (size_t j = 0; j < 10; j++) {
      Int key = rand() % 200;
      bool found_m1 = (m1.find(key) != m1.end());
      if (found_m1) m1[key];
      Elem *e = hash.Find(key);
      CHECK((e != NULL) == found_m1);
      if (found_m1)
        CHECK(m1[key] == e->val);
    }

    CHECK(m1.size() == count);
  }
}

}  // namespace xdecoder



int main() {
  for (size_t i = 0; i < 3; i++) {
    xdecoder::TestHashList<int, unsigned int>();
    xdecoder::TestHashList<unsigned int, int>();
    xdecoder::TestHashList<int16_t, int32_t>();
    xdecoder::TestHashList<int16_t, int32_t>();
    xdecoder::TestHashList<char, unsigned char>();
    xdecoder::TestHashList<unsigned char, int>();
  }
  std::cout << "Test OK.\n";
}
