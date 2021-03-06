/*
 * JSHandler.h
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "ifs/Handler.h"
#include "ifs/vm.h"
#include "Chain.h"
#include "Routing.h"

#ifndef JSHANDLER_H_
#define JSHANDLER_H_

namespace fibjs
{

class JSHandler: public Handler_base
{
    FIBER_FREE();

public:
    // object_base
    virtual result_t dispose()
    {
        return 0;
    }

public:
    // Handler_base
    virtual result_t invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                            AsyncEvent *ac);

public:
    static result_t New(v8::Local<v8::Value> hdlr,
                        obj_ptr<Handler_base> &retVal)
    {
        if (hdlr->IsString() || hdlr->IsStringObject() ||
                hdlr->IsNumberObject() || hdlr->IsRegExp() ||
                (!hdlr->IsFunction() && !hdlr->IsObject()))
            return CHECK_ERROR(CALL_E_BADVARTYPE);

        retVal = Handler_base::getInstance(hdlr);
        if (retVal)
            return 0;

        if (hdlr->IsArray())
        {
            v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(hdlr);

            obj_ptr<Chain_base> chain = new Chain();
            result_t hr = chain->append(a);
            if (hr < 0)
                return hr;

            retVal = chain;
            return 0;
        }

        if (!hdlr->IsFunction())
        {
            v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(hdlr);

            obj_ptr<Routing_base> r = new Routing();
            result_t hr = r->append(o);
            if (hr < 0)
                return hr;

            retVal = r;
            return 0;
        }

        obj_ptr<JSHandler> r = new JSHandler();
        r->SetPrivate("handler", hdlr);

        retVal = r;
        return 0;
    }

public:
    static result_t js_invoke(Handler_base *hdlr, object_base *v,
                              obj_ptr<Handler_base> &retVal, AsyncEvent *ac);
};

} /* namespace fibjs */
#endif /* JSHANDLER_H_ */
