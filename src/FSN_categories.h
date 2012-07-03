//
//  FSN_categories.h
//  FSN
//
//  Created by George King on 7/18/11.
//  Copyright 2011-2012 Foursquare Labs, Inc. All rights reserved.
//  Permission to use this file is granted in FSNetworking/license.txt (apache v2).
//


// miscellaneous support macros, categories.

#import <Foundation/Foundation.h>

// get a true binary value from an expression
#define BIT(x) ((x) ? 1 : 0)

// get a "Y" or "N" string from the boolean value of an expression
#define BIT_YN(x) ((x) ? @"Y" : @"N")


// internal logging, conditionally enabled
#if FSN_LOG
#define FSNLog(...) NSLog(@"FSN: " __VA_ARGS__)
#define FSNErr(...) fprintf(stderr, "%s\n", [[NSString stringWithFormat:__VA_ARGS__] UTF8String])
#else
#define FSNLog(...) ((void)0)
#define FSNErr(...) ((void)0)
#endif

// internal verbose logging, conditionally enabled
#if FSN_LOG_VERBOSE
#define FSNVerbose(...) NSLog(@"FSNV: " __VA_ARGS__)
#else
#define FSNVerbose(...) ((void)0)
#endif

// always log errors
#define FSNLogError0(string)        NSLog(@"ERROR: %s: " string, __FUNCTION__)
#define FSNLogError(format, ...)    NSLog(@"ERROR: %s: " format, __FUNCTION__, __VA_ARGS__)


// quick assertions to make sure we are on the expected thread

#define ASSERT_MAIN_THREAD \
NSAssert1([NSThread isMainThread], @"%@: must be called from the main thread", __FUNCTION__)

#define ASSERT_NOT_MAIN_THREAD \
NSAssert1(![NSThread isMainThread], @"%@: must be called from a background thread", __FUNCTION__)


// HTTP 1.1 code classes; not currently used
typedef enum {
    FSNHTTPCodeClassUnknown     = 0,
    FSNHTTPCodeClassInfo        = 1, // 1xx: Informational
    FSNHTTPCodeClassSuccess     = 2, // 2xx: Successful
    FSNHTTPCodeClassRedirect    = 3, // 3xx: Redirection
    FSNHTTPCodeClassError       = 4, // 4xx: Error
    FSNHTTPCodeClassServerError = 5, // 5xx: Server Error
} FSNHTTPCodeClass;


// error codes for FSNConnectionErrorDomain
typedef enum {
    FSNConnectionErrorCodeUnknown = 0,
    FSNConnectionErrorCodeJSONResultType,          // JSON parsing returned a root object of unexpected type, e.g. array instead of dict
    FSNConnectionErrorCodeExpiredInBackgroundTask, // OS expired the background task for the request
} FSNConnectionErrorCode;


extern NSString * const FSNConnectionErrorDomain;


@interface NSDictionary (FSN)

// creat a url-encoded query string; used to generate either a GET query or url-encoded POST body
- (NSString *)urlQueryString;

@end



@interface NSData (FSN)

// convenience method to ensure that top-level json object is a dictionary
- (id)dictionaryFromJSONWithError:(NSError **)error; // error pointer must not be nil

- (NSString *)stringFromUTF8;

// create a printable string with utf8 data, converting certain bytes into mnemonic characters
- (NSString *)debugString;

@end


@interface NSString (FSN)

// create a string from utf8 data
+ (NSString *)withUTF8Data:(NSData *)data;

// percent-escaped string
- (NSString *)urlEncodedString;

- (NSData *)UTF8Data;

@end



@interface NSURLResponse (FSN)

- (NSInteger)statusCode; // provide default implementation underneath NSHTTPURLResponse; returns -1

@end
