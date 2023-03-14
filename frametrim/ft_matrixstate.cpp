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

#include "ft_matrixstate.hpp"

#include <cstring>
#include <GL/gl.h>

using std::make_shared;

namespace frametrim {

MatrixState::MatrixState(MatrixState::Pointer parent):
    UsedObject(0),
    m_parent(parent)
{

}

void
MatrixState::selectMatrixType(const trace::Call& call)
{
    m_type_select_call = trace2call(call);
}

void
MatrixState::setMatrix(const trace::Call &call)
{
    assert(!strcmp(call.name(), "glLoadIdentity") ||
           !strncmp(call.name(), "glLoadMatrix", 12));

    /* Remember matrix type when doing
     * glMatrixMode
     * glPushMatrix
     * glLoad*
     */
    if (!m_type_select_call && m_parent)
        m_type_select_call = m_parent->m_type_select_call;

    m_parent = nullptr;
    setCall(trace2call(call));
    if (m_type_select_call)
        addCall(m_type_select_call);
}

AllMatrisStates::AllMatrisStates()
{
    m_mv_matrix.push(make_shared<MatrixState>(nullptr));
    m_current_matrix = m_mv_matrix.top();
    m_current_matrix_stack = &m_mv_matrix;
}

void AllMatrisStates::emitStateTo(CallSet& list) const
{
    if (!m_mv_matrix.empty())
        m_mv_matrix.top()->emitCallsTo(list);

    if (!m_proj_matrix.empty())
        m_proj_matrix.top()->emitCallsTo(list);

    if (!m_texture_matrix.empty())
        m_texture_matrix.top()->emitCallsTo(list);

    if (!m_color_matrix.empty())
        m_color_matrix.top()->emitCallsTo(list);
}

void
AllMatrisStates::loadIdentity(const trace::Call& call)
{
    return m_current_matrix->setMatrix(call);
}

void
AllMatrisStates::loadMatrix(const trace::Call& call)
{
    return m_current_matrix->setMatrix(call);
}

void
AllMatrisStates::matrixMode(const trace::Call& call)
{
    switch (call.arg(0).toUInt()) {
    case GL_MODELVIEW:
        m_current_matrix_stack = &m_mv_matrix;
        break;
    case GL_PROJECTION:
        m_current_matrix_stack = &m_proj_matrix;
        break;
    case GL_TEXTURE:
        m_current_matrix_stack = &m_texture_matrix;
        break;
    case GL_COLOR:
        m_current_matrix_stack = &m_color_matrix;
        break;
    default:
        assert(0 && "Unknown matrix mode");
    }

    if (m_current_matrix_stack->empty())
        m_current_matrix_stack->push(make_shared<MatrixState>(nullptr));

    m_current_matrix = m_current_matrix_stack->top();
    m_current_matrix->selectMatrixType(call);
}

void
AllMatrisStates::popMatrix(const trace::Call& call)
{
    m_current_matrix->addCall(trace2call(call));
    m_current_matrix_stack->pop();
    assert(!m_current_matrix_stack->empty());
    m_current_matrix = m_current_matrix_stack->top();
}

void
AllMatrisStates::pushMatrix(const trace::Call& call)
{
    m_current_matrix = make_shared<MatrixState>(m_current_matrix);
    m_current_matrix_stack->push(m_current_matrix);
    m_current_matrix->addCall(trace2call(call));
}

void AllMatrisStates::matrixOp(const trace::Call& call)
{
    m_current_matrix->addCall(trace2call(call));
}

}
