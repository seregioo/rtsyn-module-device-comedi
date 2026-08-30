#include <gtest/gtest.h>
#include <rtsyn/module_loader.h>

#ifndef RTSYN_TEST_MODULE_PATH
#error RTSYN_TEST_MODULE_PATH must contain the built device module path
#endif

TEST(DeviceModuleLoader, LoadsAnAbiNodeDescriptor) {
    rtsyn_module_loader_t *loader = rtsyn_module_loader_create(RTSYN_TEST_MODULE_PATH);
    ASSERT_NE(loader, nullptr);

    const rtsyn_abi_node_descriptor_t *descriptor = rtsyn_module_loader_get_descriptor(loader);
    ASSERT_NE(descriptor, nullptr);
    ASSERT_NE(descriptor->name, nullptr);
    EXPECT_STREQ(descriptor->name, "comedi");
    EXPECT_EQ(descriptor->node_type, RTSYN_ABI_NODE_DEVICE);

    rtsyn_module_loader_destroy(loader);
}
