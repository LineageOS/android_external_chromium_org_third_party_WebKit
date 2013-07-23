/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/crypto/CryptoOperation.h"

#include "bindings/v8/ExceptionState.h"
#include "bindings/v8/custom/V8ArrayBufferCustom.h" // MUST precede ScriptPromiseResolver for compilation to work.
#include "bindings/v8/ScriptPromiseResolver.h"
#include "core/dom/ExceptionCode.h"
#include "modules/crypto/Algorithm.h"
#include "public/platform/WebArrayBuffer.h"
#include "public/platform/WebCrypto.h"
#include "wtf/ArrayBuffer.h"
#include "wtf/ArrayBufferView.h"

namespace WebCore {

CryptoOperation::~CryptoOperation()
{
    abortImpl();
    ASSERT(!m_impl);
}

PassRefPtr<CryptoOperation> CryptoOperation::create(const WebKit::WebCryptoAlgorithm& algorithm, ExceptionState* es)
{
    return adoptRef(new CryptoOperation(algorithm, es));
}

CryptoOperation::CryptoOperation(const WebKit::WebCryptoAlgorithm& algorithm, ExceptionState* es)
    : m_algorithm(algorithm)
    , m_impl(0)
    , m_exceptionState(es)
    , m_state(Initializing)
{
    ASSERT(es);
    ScriptWrappable::init(this);
}

CryptoOperation* CryptoOperation::process(ArrayBuffer* data)
{
    process(static_cast<unsigned char*>(data->data()), data->byteLength());
    return this;
}

CryptoOperation* CryptoOperation::process(ArrayBufferView* data)
{
    process(static_cast<unsigned char*>(data->baseAddress()), data->byteLength());
    return this;
}

ScriptObject CryptoOperation::finish()
{
    switch (m_state) {
    case Initializing:
        ASSERT_NOT_REACHED();
        return ScriptObject();
    case Processing:
        m_state = Finishing;
        // NOTE: The following line can result in re-entrancy to |this|
        m_impl->finish();
        break;
    case Finishing:
        // Calling finish() twice is a no-op.
        break;
    case Done:
        // Calling finish() after already complete is a no-op.
        ASSERT(!m_impl);
        break;
    }

    return promiseResolver()->promise();
}

ScriptObject CryptoOperation::abort()
{
    if (abortImpl())
        promiseResolver()->reject(ScriptValue::createNull());
    return promiseResolver()->promise();
}

Algorithm* CryptoOperation::algorithm()
{
    if (!m_algorithmNode)
        m_algorithmNode = Algorithm::create(m_algorithm);
    return m_algorithmNode.get();
}

void CryptoOperation::initializationFailed()
{
    ASSERT(m_state == Initializing);

    m_exceptionState->throwDOMException(NotSupportedError);

    m_exceptionState = 0;
    m_state = Done;
}

void CryptoOperation::initializationSucceded(WebKit::WebCryptoOperation* operationImpl)
{
    ASSERT(m_state == Initializing);
    ASSERT(operationImpl);
    ASSERT(!m_impl);

    m_exceptionState = 0;
    m_impl = operationImpl;
    m_state = Processing;
}

void CryptoOperation::completeWithError()
{
    ASSERT(m_state == Processing || m_state == Finishing);

    m_impl = 0;
    m_state = Done;

    promiseResolver()->reject(ScriptValue::createNull());
}

void CryptoOperation::completeWithArrayBuffer(const WebKit::WebArrayBuffer& buffer)
{
    ASSERT(m_state == Processing || m_state == Finishing);

    m_impl = 0;
    m_state = Done;

    promiseResolver()->fulfill(PassRefPtr<ArrayBuffer>(buffer));
}

void CryptoOperation::process(const unsigned char* bytes, size_t size)
{
    switch (m_state) {
    case Initializing:
        ASSERT_NOT_REACHED();
    case Processing:
        m_impl->process(bytes, size);
        break;
    case Finishing:
    case Done:
        return;
    }
}

bool CryptoOperation::abortImpl()
{
    switch (m_state) {
    case Initializing:
        ASSERT_NOT_REACHED();
        break;
    case Processing:
    case Finishing:
        // This will cause m_impl to be deleted.
        m_state = Done;
        m_impl->abort();
        m_impl = 0;
        return true;
    case Done:
        ASSERT(!m_impl);
        break;
    }

    return false;
}

ScriptPromiseResolver* CryptoOperation::promiseResolver()
{
    if (!m_promiseResolver)
        m_promiseResolver = ScriptPromiseResolver::create();
    return m_promiseResolver.get();
}

} // namespace WebCore