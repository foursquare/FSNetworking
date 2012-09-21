//
//  FSNConnection.h
//  FSN
//
//  Created by George King on 7/14/11.
//  Copyright 2011-2012 Foursquare Labs, Inc. All rights reserved.
//  Permission to use this file is granted in FSNetworking/license.txt (apache v2).
//

// FSNConnection encapsulates a complete HTTP connection.

// connection semantics:

// completion: NSURLConnection success or failure (including timeout).
// success: no error set; an error may be set even though the underlying NSURLConnection succeeded.

// block semantics:

// once the connection is started, all blocks are guaranteed to be cleared on completion or cancellation.
// therefore, retain cycles involving the connection and the blocks will eventually be broken.

// unless the connection is canceled, its completionBlock will be called (on completion or failure).
// if an error occurs at any point, the connection's error parameter is set, and the completionBlock is called.

// if the underlying connection succeeds, and a parseBlock is specified,
// then parseBlock is called in a background thread.
// if parseBlock sets the error pointer, then connection.error is set to that error.
// regardless of error, connection.parseResult is set to the object returned from the parseBlock,
// and the completionBlock is called.
// if no parseBlock is specified, then completionBlock is called immediately.

// both progressBlock and completionBlock are always called on the main thread.


#import "FSN_categories.h"
#import "FSNData.h"


@class FSNConnection;

// block types
typedef id(^FSNParseBlock)(FSNConnection *, NSError **);
typedef void(^FSNCompletionBlock)(FSNConnection *);
typedef void(^FSNProgressBlock)(FSNConnection *);


// http methods
// add additional http methods as necessary
// non-http request methods could also go here
typedef enum {
    FSNRequestMethodGET = 0, // default
    FSNRequestMethodPOST,
} FSNRequestMethod;


// NSNotification names
extern NSString * const FSNConnectionActivityBegan;
extern NSString * const FSNConnectionActivityEnded;

// string function for request enum
NSString* stringForRequestMethod(FSNRequestMethod method);


@interface FSNConnection : NSObject <NSURLConnectionDelegate>

// NOTE: object property declarations in header are explicity 'strong' so that non-arc code may include the header.

@property (nonatomic, strong) NSURL *url;
@property (nonatomic) FSNRequestMethod method;

#if TARGET_OS_IPHONE
@property (nonatomic) BOOL shouldRunInBackground; // defaults to YES for POST method
#endif

@property (nonatomic, strong) NSDictionary *headers;    // optional custom http headers
@property (nonatomic, strong) NSDictionary *parameters; // GET or POST parameters, including POST form data

@property (nonatomic, copy) FSNParseBlock parseBlock;           // executed in background thread
@property (nonatomic, copy) FSNCompletionBlock completionBlock; // executed in main thread
@property (nonatomic, copy) FSNProgressBlock progressBlock;     // executed in main thread

@property (nonatomic, strong, readonly) NSURLResponse *response;        // response from NSURLConnection
@property (nonatomic, strong, readonly) NSHTTPURLResponse *httpResponse; // response or nil if not an http response
@property (nonatomic, strong, readonly) NSData *responseData;           // populated with data unless responseStream is set.
@property (nonatomic, strong) NSOutputStream *responseStream;           // if this is set then responseData will be nil

@property (nonatomic, strong, readonly) id<NSObject> parseResult;       // result of parseBlock; may be nil on success
@property (nonatomic, strong, readonly) NSError *error;                 // if set then the request or parse failed

@property (nonatomic, readonly) BOOL didStart;          // start was called
@property (nonatomic, readonly) BOOL didFinishLoading;  // underlying connection finished loading
@property (nonatomic, readonly) BOOL didComplete;       // underlying connection either finished or failed
@property (nonatomic, readonly) BOOL didSucceed;        // finished with no error

@property (nonatomic, readonly) long long uploadProgressBytes;
@property (nonatomic, readonly) long long uploadExpectedBytes;
@property (nonatomic, readonly) long long downloadProgressBytes;
@property (nonatomic, readonly) long long downloadExpectedBytes;
@property (nonatomic, readonly) float uploadProgress;
@property (nonatomic, readonly) float downloadProgress;

@property (nonatomic, readonly) int concurrencyCountAtStart;
@property (nonatomic, readonly) NSTimeInterval startTime;
@property (nonatomic, readonly) NSTimeInterval challengeInterval;
@property (nonatomic, readonly) NSTimeInterval responseInterval;
@property (nonatomic, readonly) NSTimeInterval finishOrFailInterval;
@property (nonatomic, readonly) NSTimeInterval parseInterval;


+ (id)withUrl:(NSURL *)url
       method:(FSNRequestMethod)method
      headers:(NSDictionary *)headers
   parameters:(NSDictionary *)parameters
   parseBlock:(FSNParseBlock)parseBlock
completionBlock:(FSNCompletionBlock)completionBlock
progressBlock:(FSNProgressBlock)progressBlock;


+ (NSSet *)connections;
+ (void)cancelAllConnections;


// call this method to allow the request to complete but ignore the response.
- (void)clearBlocks;

- (FSNConnection *)start; // returns self for chaining convenience, or nil if start fails.

- (void)cancel; // no blocks will be called after request is cancelled, unless a call to finish is already scheduled.

@end

