/* Pixel upload footprints must include the GLES2 half-float token. */

#include "gtest/gtest.h"
#include "glproc.hpp"
#include "glsize.hpp"

static void APIENTRY
getInteger(GLenum pname, GLint *value)
{
    *value = pname == GL_UNPACK_ALIGNMENT ? 1 : 0;
}

PFN_GLGETINTEGERV _glGetIntegerv = getInteger;

TEST(glsize, half_float_oes_upload_footprint)
{
    EXPECT_EQ(8u, _gl_type_size(GL_HALF_FLOAT_OES, 4));
    EXPECT_EQ(280u, _gl_image_size(GL_RGBA, GL_HALF_FLOAT_OES, 7, 5, 1, false));
    EXPECT_EQ(840u, _gl_image_size(GL_RGBA, GL_HALF_FLOAT_OES, 7, 5, 3, false));
    EXPECT_EQ(_gl_image_size(GL_RGBA, GL_HALF_FLOAT, 7, 5, 3, false),
              _gl_image_size(GL_RGBA, GL_HALF_FLOAT_OES, 7, 5, 3, false));
}

int
main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
