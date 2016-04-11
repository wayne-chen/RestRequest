# RestRequest
JUCE module for making HTTP requests to REST API's

### Example Usage

First declare your class instance e.g. as a member variable:
```
adamski::RestRequest request;
```

Then in your constructor add any headers you will need across different requests e.g.:
```
request.header ("Content-Type", "application/json");
request.header ("Authorization", "Basic " + Base64::toBase64 ("username:password"));
```

Example `GET` request:

```C++
adamski::RestRequest::Response response = request.get ("registry/_design/module-views/_view/all-modules")
        .execute();

// Example response debugging

DBG (response.bodyAsString);
DBG (response.result.getErrorMessage());
        
for (auto key : response.headers.getAllKeys())
{
        DBG (key << ": " << response.headers.getValue(key, "n/a"));
}
```

Example `POST` request:

```
adamski::RestRequest::Response response = request.post ("registry/_find")
        .field ("selector", propertyAsVar("$text", searchString))
        .execute();
```

Example 'PUT' request with chained field methods:

```
adamski::RestRequest::Response response = request.put ("_users/" + id)
        .field ("type", "user")
        .field ("name", username)
        .field ("email", email)
        .field ("roles", Array<var>({var("publisher")}))
        .field ("password_sha", stringToSHA1 (password + salt))
        .field ("salt", salt)
        .execute();

```
