/*
  ==============================================================================

    RestRequest.h
    Created: 16 Dec 2015 2:29:23pm
    Author:  Adam Wilson

  ==============================================================================
*/

#ifndef RESTREQUEST_H_INCLUDED
#define RESTREQUEST_H_INCLUDED

#include <juce_core/juce_core.h>

class RestRequest
{
public:

    RestRequest (juce::String urlString) : url (urlString) {}
    RestRequest (juce::URL url)          : url (url) {}
    RestRequest () {}

    struct Response
    {
        juce::Result result;
        juce::StringPairArray headers;
        juce::var body;
        juce::String bodyAsString;
        int status;

        Response() : result (juce::Result::ok()), status (0) {} // not sure about using Result if we have to initialise it to ok...
    } response;


    RestRequest::Response execute ()
    {
        auto urlRequest = url.getChildURL (endpoint);
        bool hasFields = (fields.getProperties().size() > 0);
        if (hasFields)
        {
            MemoryOutputStream output;

            fields.writeAsJSON (output, 0, false, 20);
            urlRequest = urlRequest.withPOSTData (output.toString());
        }

        std::unique_ptr<juce::InputStream> input (urlRequest.createInputStream (hasFields, nullptr, nullptr, stringPairArrayToHeaderString(headers), 0, &response.headers, &response.status, 5, verb));

        response.result = checkInputStream (input);
        if (response.result.failed()) return response;

        response.bodyAsString = input->readEntireStreamAsString();
        response.result = juce::JSON::parse(response.bodyAsString, response.body);

        return response;
    }


    RestRequest get (const juce::String& endpoint)
    {
        RestRequest req (*this);
        req.verb = "GET";
        req.endpoint = endpoint;

        return req;
    }

    RestRequest post (const juce::String& endpoint)
    {
        RestRequest req (*this);
        req.verb = "POST";
        req.endpoint = endpoint;

        return req;
    }

    RestRequest put (const juce::String& endpoint)
    {
        RestRequest req (*this);
        req.verb = "PUT";
        req.endpoint = endpoint;

        return req;
    }

    RestRequest del (const juce::String& endpoint)
    {
        RestRequest req (*this);
        req.verb = "DELETE";
        req.endpoint = endpoint;

        return req;
    }

    RestRequest field (const juce::String& name, const juce::var& value)
    {
        fields.setProperty(name, value);
        return *this;
    }

    RestRequest header (const juce::String& name, const juce::String& value)
    {
        RestRequest req (*this);
        headers.set (name, value);
        return req;
    }

    const juce::URL& getURL() const
    {
        return url;
    }

    const juce::String& getBodyAsString() const
    {
        return bodyAsString;
    }

private:
    juce::URL url;
    juce::StringPairArray headers;
    juce::String verb;
    juce::String endpoint;
    juce::DynamicObject fields;
    juce::String bodyAsString;

    juce::Result checkInputStream (std::unique_ptr<juce::InputStream>& input)
    {
        if (! input) return juce::Result::fail ("HTTP request failed, check your internet connection");
        return juce::Result::ok();
    }

    static juce::String stringPairArrayToHeaderString(juce::StringPairArray stringPairArray)
    {
        juce::String result;
        for (auto key : stringPairArray.getAllKeys())
        {
            result += key + ": " + stringPairArray.getValue(key, "") + "\n";
        }
        return result;
    }
};



#endif  // RESTREQUEST_H_INCLUDED
