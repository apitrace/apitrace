/*********************************************************************
 *
 * Copyright 2020 Collabora Ltd
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *********************************************************************/

#pragma once

#include "ft_dependecyobject.hpp"
#include <stack>

namespace frametrim {

class MatrixState : public UsedObject
{
public:
    using Pointer = std::shared_ptr<MatrixState>;

    MatrixState(Pointer parent);

    void selectMatrixType(const trace::Call& call);
    void setMatrix(const trace::Call& call);

private:
    Pointer m_parent;
    PTraceCall m_type_select_call;
};

using PMatrixState = std::shared_ptr<MatrixState>;

class AllMatrisStates {
public:

    AllMatrisStates();

    void loadIdentity(const trace::Call& call);
    void loadMatrix(const trace::Call& call);
    void matrixMode(const trace::Call& call);
    void popMatrix(const trace::Call& call);
    void pushMatrix(const trace::Call& call);
    void matrixOp(const trace::Call& call);
    void emitStateTo(CallSet& list) const;

private:
    std::stack<PMatrixState> m_mv_matrix;
    std::stack<PMatrixState> m_proj_matrix;
    std::stack<PMatrixState> m_texture_matrix;
    std::stack<PMatrixState> m_color_matrix;

    PMatrixState m_current_matrix;
    std::stack<PMatrixState> *m_current_matrix_stack;
};

}
