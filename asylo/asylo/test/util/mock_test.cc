/*
 *
 * Copyright 2018 Asylo authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "asylo/test/util/mock_enclave_client.h"
#include "asylo/test/util/mock_enclave_loader.h"

namespace asylo {
namespace {

// These tests just confirm these loaders build.

TEST(MockTest, MockEnclaveLoader) { MockEnclaveLoader loader; }

TEST(MockTest, MockEnclaveClient) { MockEnclaveClient loader; }

}  // namespace
}  // namespace asylo
