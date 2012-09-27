FSNetworking
============

### foursquare's iOS networking library


FSN is a small library for HTTP networking on iOS. It comprises a single class, FSNConnection, and several small Cocoa Foundation categories. FSNConnection uses NSConnection, blocks, and NSConnection's operation queue delegate or GCD.


## Features

FSNConnection offers the following:
- Asynchronous HTTP and HTTPS requests.
- GET and POST (urlencoded and form data); easily extended for other HTTP methods.
- Response parsing in a background thread, followed by a completion block on the main thread.
- Convenient, safe object retention sementics and life cycle management, enabling fire-and-forget requests.
- Support for iOS background tasks.
- Useful utilities for creating and debugging form data, parsing JSON, error checking, etc.
- Clean, straightforward implementation; no class hierarchies to grok. Easily trace and understand the life cycle of your connection!


## Examples

### GET

``` objective-c

NSURL *url                = ...; // required
NSDictionary *headers     = ...; // optional
NSDictionary *parameters  = ...; // optional

FSNConnection *connection =
[FSNConnection withUrl:url
                method:FSNRequestMethodGET
               headers:headers
            parameters:parameters
            parseBlock:^id(FSNConnection *c, NSError **error) {
                return [c.responseData dictionaryFromJSONWithError:error];
            }
       completionBlock:^(FSNConnection *c) {
           NSLog(@"complete: %@\n  error: %@\n  parseResult: %@\n", c, c.error, c.parseResult);
       }
         progressBlock:^(FSNConnection *c) {
             NSLog(@"progress: %@: %.2f/%.2f", c, c.uploadProgress, c.downloadProgress);
         }];

[connection start];
```

The most important aspects of this pattern are:
- The value returned from the parse block is assigned to the connection's parseResult property.
- Similarly, if any error is set by the parse block, that is assigned to the connection's error property. 
- You can write simple wrappers around this method to accommodate all the endpoints in a web API:
  - Compose a URL for each endpoint.
  - Send standard headers such as User-Agent and Accept-Language.
  - Send standard parameters such as OAuth token, client version, etc.
  - Combine standard and custom handling for parse and completion steps by calling small custom blocks inside of wrapper blocks that perform uniform error-checking, parsing, etc.

For example, the foursquare app defines a category on FSNConnection that looks like this:

``` objective-c

@implementation FSNConnection (FS)


// convenience accessor property that casts parseResult to our custom, API-specific type.
- (ApiResult *)apiResult {
    return self.parseResult;
}


- (ApiResult *)makeApiResultWithError:(NSError **)error {    
    // parse the foursquare API result JSON.
    // then check self.response.statusCode, as well as the foursquare API result 'meta' JSON dict for errors.
    // if everything is OK, then return the an ApiResult instance, which contains api-specific properties.
    // otherwise, set *error and return nil.
    ...
}


// wrap an arbitrary completionBlock with standard handling behavior.
- (void)finishWithBlock:(FSNCompletionBlock)completionBlock displayError:(BOOL)displayError {
    ASSERT_MAIN_THREAD;
    
    if (self.error) {
        // debug-build only error reporting.
        FSLog(@"request error: %@ -- %@ -- %@ -- %@",
              self.error, self.apiResult.errorDetail, self.apiResult.errorMessage, self.apiResult.errorType);
    }
    
    // perform custom block
    if (completionBlock) {
        completionBlock(self);
    }
    
    if (self.error && displayError) {
        // display standard error UI.
        ...
    }
    
    // send standard notifications last.
    ...
}


// most foursquare API requests are constructed with this method.
// it standardizes some elements of request construction, and passes through custom parameters.
// note how we wrap the custom completionBlock with standard behavior by virtue of an intermediate method;
// this allows us to precisely control when the custom callback happens.
+ (id)withEndpoint:(NSString *)endpoint
            method:(FSNRequestMethod)method
        parameters:(NSDictionary *)parameters
      displayError:(BOOL)displayError
        parseBlock:(FSNParseBlock)parseBlock
   completionBlock:(FSNCompletionBlock)completionBlock {
    
    // note: FSAPI is a singleton defining API-related properties, defined elsewhere.
    return [self withUrl:[[FSAPI shared] urlForEndpoint:endpoint]
                  method:method
                 headers:[FSAPI shared].standardRequestHeaders // headers are the same for every request
              parameters:[[FSAPI shared] completeParameters:parameters] // add standard parameters like OAuth token
              parseBlock:parseBlock
         completionBlock:^(FSNConnection *c) {
             [c finishWithBlock:completionBlock displayError:displayError];
         }
           progressBlock:nil];
}


// a second wrapper constructor standardizes parseBlock implementa
// most requests are constructed with this method.
+ (id)withEndpoint:(NSString*)endpoint
            method:(FSNRequestMethod)method
        parameters:(NSDictionary*)parameters
      displayError:(BOOL)displayError
   completionBlock:(FSNCompletionBlock)completionBlock {
    
    return [self withEndpoint:endpoint
                       method:method
                   parameters:parameters
                 displayError:displayError
                   parseBlock:^(FSNConnection *c, NSError **error) {
                       return [c makeApiResultWithError:error];
                   }
              completionBlock:completionBlock];
}

@end
```


### POST

POST requests are made using the same class and calls as GET. This uniformity is one of the most satisfying aspects of the library. POST parameter values can be any of three types: NSString, NSNumber, and FSNData.
- If the POST consists of only NSString and NSNumber values, then it will have the content-type "application/x-www-form-urlencoded".
- If any parameter is an FSNData object, then the request will have the content-type "multipart/form-data".

This design allows us to rapidly adjust to changing web API requirements with minimal code changes, and eases form request implementation.

As an example, a photo upload might look like this:

``` objective-c

UIImage *originalImage = ...;

// form file name and parameter name would be determined by the web API
NSDictionary *parameters =
[NSDictionary dictionaryWithObjectsAndKeys:
 [FSNData withImage:originalImage jpegQuality:.75 fileName:@"fileName"],  @"paramName",
 nil];

FSNConnection *connection =
[FSNConnection withUrl:url
                method:FSNRequestMethodPOST
               headers:headers
            parameters:parameters
            parseBlock:nil
       completionBlock:nil
         progressBlock:nil];
```

The FSNData class has several other constructors for sending raw NSData; please see the header for more details.
MIME types are represented as an enumeration to encourage standards-compliance and reduce the risk of typos in string literals. Currently, only two MIME types are enumerated, but more can be added easily; just define additional enumerations and their corresponding strings in FSNData.h and FSNData.m.


### Other HTTP Methods.

Other HTTP methods like HEAD and PUT are not yet supported, but adding them should not be hard. Patches are welcome; feel free to get in touch if you would like to discuss the implementation.


## Demos


FSNDemo-iOS shows how to set up a single connection to the foursquare API.
- As is, the connection will fail with an OAuth error, demonstrating the error handling conventions.
- To see the connection succeed, sign up for an OAuth token at http://developer.foursquare.com, or else just request some static html page.

FSNDemo-mac shows how to make the exact same connection, but from the command line.
- Running an asynchronous connection requires the program to run the main runloop manually.


## License

FSN is released under the Apache License, Version 2.0; see license.txt. More information can be found here:

- http://www.apache.org/licenses/LICENSE-2.0
- http://www.opensource.org/licenses/apache-2.0
- http://en.wikipedia.org/wiki/Apache_License
- http://www.gnu.org/licenses/license-list.html#apache2


## Releases

The current release is 1.0. This code has been in production in the foursquare app for many moons.


## Known Issues

### Delegate Queues

Support for NSURLConnection's setDelegateQueue exists but is disabled by default because it causes iOS 5 applications to deadlock. Instead, FSN uses the main thread for connection callbacks and GCD to perform parsing on a background thread. Define FSN_QUEUED_CONNECTIONS to 1 (typically in your prefix header) to use delegate queues.

Since delegate queues appear to work in Lion, the Mac demo does use delegate queues. However, this has been tested only minimally. If you enable this and find bugs, please submit patches!

See also:
- http://web.archiveorange.com/archive/v/x0fiW83vGyS2Rb5ttxbC
- http://www.ddeville.me/2011/12/broken-NSURLConnection-on-ios/

### Recursive Lock

An NSRecursiveLock is used to guard the parseBlock against cancellation/deallocation while in concurrent usage. We would prefer a lock-free implementation for the sake of simplicity, and we welcome any scrutiny or suggestions for a better solution.


## Dependencies

FSNetworking depends only on Cocoa's Foundation framework; convenience methods using UIKit are guarded appropriately. We currently build against iOS 5.0 with the latest public release of the Xcode toolset. The Mac demo builds against OS X 10.7.


## Maintainers

FSNConnection was initially developed by Foursquare Labs as a replacement for ASIHTTPRequest in our iOS application. We now use it for all HTTP networking in the foursquare iOS app.

The current maintainer is:

- Bryan Bonczek bryan@foursquare.com

Feedback, bug reports, and code contributions are all welcome!

