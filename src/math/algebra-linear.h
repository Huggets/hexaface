/**
 * \file algebra-linear
 * \brief Implement things like vector and matrix.
 */
#pragma once

/**
 * \brief Replace by an identity matrix for HxfMat4
 */
#define HXF_MAT4_IDENTITY {{\
    {1.0f, 0.0f, 0.0f, 0.0f},\
    {0.0f, 1.0f, 0.0f, 0.0f},\
    {0.0f, 0.0f, 1.0f, 0.0f},\
    {0.0f, 0.0f, 0.0f, 1.0f}}}

/**
 * \struct HxfVec2
 * \brief A 2 dimensional vector of float values.
 */
typedef struct HxfVec2 {
    float x;
    float y;
} HxfVec2;

/**
 * \struct HxfVec3
 * \brief A 3 dimensional vector of float values.
 */
typedef struct HxfVec3 {
    float x;
    float y;
    float z;
} HxfVec3;

/**
 * \struct HxfVec4
 * \brief A 4 dimensional vector of float values.
 */
typedef struct HxfVec4 {
    float x;
    float y;
    float z;
    float w;
} HxfVec4;

/**
 * \struct HxfMat4
 * \brief A matrix of 4 rows and 4 columns of float values.
 */
typedef struct HxfMat4 {
    float mat[4][4];
} HxfMat4;

/**
 * \brief Return an identity matrix.
 * \return An identity HxfMat4.
 */
HxfMat4 hxfMat4Identity();

/* OPERATION */

/**
 * \brief Multiply a HxfMat4 by a HxfMat4.
 * 
 * It does a * b and return the result.
 * 
 * \return a * b.
 */
HxfMat4 hxfMat4MulMat(HxfMat4 * a, HxfMat4 * b);

/**
 * \brief Multiply a HxfMat4 by a HxfVec4.
 * 
 * It does a * b and return the result.
 * 
 * \return a * b.
 */
HxfVec4 hxfMat4MulVec(HxfMat4 * a, HxfVec4 * b);

/* TRANSFORMATION */

/**
 * \brief Scale the matrix by factor.
 * 
 * TODO: Write the doc.
 */
void hxfMat4Scale(HxfMat4 * mat4, HxfVec3 * factor);

/**
 * \brief Translation
 * 
 * TODO: Write the doc
 */
void hxfMat4Translate(HxfMat4 * mat4, HxfVec3 * factor);

/**
 * \brief Rotation
 * 
 * angle in radians
 * 
 * TODO: Write the doc
 */
HxfMat4 hxfMat4Rotate(HxfMat4 * mat4, float angle, HxfVec3 * position);

HxfMat4 hxfPerspectiveProjection(float near, float far, float fov, float aspect);