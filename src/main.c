#include <stdlib.h>
#include "vulkan/vulkan.h"

#include "math/algebra-linear.h"
#include <stdio.h>

// static void printm(HxfMat4 mat) {
//     for (int x = 0; x != 4; x++) {
//         for (int y = 0; y != 4; y++) {
//             printf("%i\t", (int)mat.mat[x][y]);
//         }
//         printf("\n");
//     }
// }

// static void printv(HxfVec4 vec) {
//     printf("%f %f %f %f\n", vec.x, vec.y, vec.z, vec.w);
// }

int main(int argc, char ** argv) {
    // HxfVec4 vec = {1.0f, 0.0f, 0.0f, 1.0f};
    // HxfMat4 trans = HXF_MAT4_IDENTITY;
    // HxfVec3 transVec = {1.0f, 1.0f, 0.0f};
    // hxfMat4Translate(&trans, &transVec);
    // vec = hxfMat4MulVec(&trans, &vec);

    // printv(vec);

    HxfVulkanInstance instance;

    hxfInitVulkan(&instance);
    hxfRunVulkan(&instance);
    hxfDestroyVulkan(&instance);

    return EXIT_SUCCESS;
}