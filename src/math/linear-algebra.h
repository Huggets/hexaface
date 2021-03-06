#pragma once

#include <stdint.h>

/**
 * @brief A 4×4 identity matrix.
 */
#define HXF_MAT4_IDENTITY (HxfMat4){{\
    {1.0f, 0.0f, 0.0f, 0.0f},\
    {0.0f, 1.0f, 0.0f, 0.0f},\
    {0.0f, 0.0f, 1.0f, 0.0f},\
    {0.0f, 0.0f, 0.0f, 1.0f}}}

/**
 * @struct HxfVec2
 * @brief A 2 dimensional vector of float values.
 */
typedef struct HxfVec2 {
    float x;
    float y;
} HxfVec2;

/**
 * @struct HxfIvec2
 * @brief A 2 dimensional vector of int values.
 */
typedef struct HxfIvec2 {
    int32_t x;
    int32_t y;
} HxfIvec2;

/**
 * @struct HxfVec3
 * @brief A 3 dimensional vector of float values.
 */
typedef struct HxfVec3 {
    float x;
    float y;
    float z;
} HxfVec3;

/**
 * @struct HxfIvec3
 * @brief A 3 dimensional vector of integer values.
 */
typedef struct HxfIvec3 {
    int32_t x;
    int32_t y;
    int32_t z;
} HxfIvec3;

/**
 * @struct HxfVec4
 * @brief A 4 dimensional vector of float values.
 */
typedef struct HxfVec4 {
    float x;
    float y;
    float z;
    float w;
} HxfVec4;

/**
 * \struct HxfMat4
 * \brief A 4×4 matrix of float value.
 */
typedef struct HxfMat4 {
    float mat[4][4];
} HxfMat4;

/* OPERATION */

/**
 * @brief Multiply a HxfMat4 by a HxfMat4.
 *
 * It does a × b and return the result.
 *
 * @param a A pointer to a HxfMat4 that is the left component of the multiplication.
 * @param b A pointer to a HxfMat4 that is the right component of the multiplication.
 *
 * @return A HxfMat4 that is the result of a × b.
 */
HxfMat4 hxfMat4MulMat(const HxfMat4* restrict a, const HxfMat4* restrict b);

/**
 * @brief Multiply a HxfMat4 by a HxfVec4.
 *
 * It does a × b and return the result.
 *
 * @param a A pointer to a HxfMat4 that is the left component of the multiplication.
 * @param b A pointer to a HxfVec4 that is the right component of the multiplication.
 *
 * @return A HxfVec4 that is the result of a × b.
 */
HxfVec4 hxfMat4MulVec(const HxfMat4* restrict a, const HxfVec4* restrict b);

/**
 * @brief Do a cross product between a and b.
 *
 * @param a A pointer to a HxfVec3 that is the left component of the cross product.
 * @param b A pointer to a HxfVec3 that is the right component of the cross product.
 *
 * @return A HxfVec3 that is the result of the cross product.
 */
HxfVec3 hxfVec3Cross(const HxfVec3* restrict a, const HxfVec3* restrict b);

/**
 * @brief Do a dot product between a and b.
 *
 * @param a A pointer to a HxfVec3 that is the left component of the dot product.
 * @param b A pointer to a HxfVec3 that is the right component of the dot product.
 *
 * @return A float that is the result of the dot product.
 */
float hxfVec3Dot(const HxfVec3* restrict a, const HxfVec3* restrict b);

/**
 * @brief Normalize the HxfVec3.
 *
 * @param a The HxfVec3 to normalize.
 *
 * @return A HxfVec3 that is the normalized vector.
 */
HxfVec3 hxfVec3Normalize(const HxfVec3* restrict a);

/**
 * @brief Add two HxfVec3.
 *
 * Do a + b and return the result.
 *
 * @param a A pointer to the HxfVec3 that is the left component of the addition.
 * @param b A pointer to the HxfVec3 that is the right component of the addition.
 *
 * @return A HxfVec3 that is the result of addition.
 */
HxfVec3 hxfVec3Add(const HxfVec3* restrict a, const HxfVec3* restrict b);

/**
 * @brief Substract two HxfVec3.
 *
 * Do a - b and return the result.
 *
 * @param a A pointer to the HxfVec3 that is the left component of the substraction.
 * @param b A pointer to the HxfVec3 that is the right component of the substraction.
 *
 * @return A HxfVec3 that is the result of substraction.
 */
HxfVec3 hxfVec3Sub(const HxfVec3* restrict a, const HxfVec3* restrict b);

/**
 * @brief Round the vector’s value to the smallest value.
 *
 * e.g: 2.1 become 2, 6.8 become 6, -3.3 become -4 and -8.9 become -9.
 *
 * @param vec The vector to round.
 *
 * @return The rounded vector.
 */
HxfIvec3 roundVector(HxfVec3* restrict vec);

/* TRANSFORMATION */

/**
 * @brief Return a HxfMat4 that scale by factor.
 */
HxfMat4 hxfMat4ScaleMatrix(const HxfVec3* restrict factor);

/**
 * @brief Return a HxfMat4 that translate by factor.
 */
HxfMat4 hxfMat4TranslationMatrix(const HxfVec3* restrict factor);

/**
 * @brief Return a rotation matrix that rotate along the axis.
 *
 * @return The rotation matrix.
 */
HxfMat4 hxfMat4RotationMatrix(float angle, const HxfVec3* restrict axis);

/**
 * @brief Return a perspective projection matrix.
 *
 * @param near The near plane.
 * @param far The far plane.
 * @param fov The vertical field of view of the projection.
 * @param aspect The aspect ratio of the projection.
 *
 * @return The perspective projection.
 */
HxfMat4 hxfPerspectiveProjectionMatrix(float near, float far, float fov, float aspect);

/**
 * @brief Return a view matrix.
 *
 * @param position The position of the view.
 * @param forwardDirection The direction vector that point where the view is looking.
 * @param upDirection The up vector, pointing to the top.
 *
 * @return The view matrix.
 */
HxfMat4 hxfViewMatrix(const HxfVec3* restrict position, const HxfVec3* restrict forwardDirection, const HxfVec3* restrict upDirection);