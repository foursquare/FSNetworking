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


// shorthand for the cocoa init idiom
#define INIT(...) self = (__VA_ARGS__); if (!self) return nil;


// shorthand for checking class membership
#define IS_KIND(obj, class_name) ([(obj) isKindOfClass:[class_name class]])

#define IS_KIND_OR_NIL(obj, class_name) \
({ id _obj = (obj); (!_obj || IS_KIND(_obj, class_name))


// return the object if it is of the specified class, or else nil
#define KIND_OR_NIL(obj, class_name) \
({ class_name *_obj = (id)(obj); (IS_KIND((_obj), class_name) ? (_obj) : nil); })


// return the object if it is of the specified class, or else NSNull
// this is useful for specifying values for dictionaryWithObjectsForKeys:
#define KIND_OR_NULL(obj, class_name) \
({ id _obj = (obj); (IS_KIND((_obj), class_name) ? (_obj) : [NSNull null]); })


// return the string if it is not nil, otherwise return the blank string
#define STRING_OR_BLANK(str) \
({ NSString* _str = (str); _str ? _str : @""; })


// return the string preceded by a space if not nil, otherwise the blank string
#define STRING_WITH_SPACE_PREFIX_OR_BLANK(str) \
({ NSString* _str = (str); _str ? [@" " stringByAppendingString:_str] : @""; })


// check if an object is nil or NSNull
#define IS_NIL_OR_NULL(obj) \
({ id _obj = (obj); !_obj || IS_KIND(_obj, NSNull); })


// return the object if it is non-nil, or else return NSNull
#define LIVE_OR_NULL(obj) \
({ id _obj = (obj); _obj ? _obj : [NSNull null]; })


// return the object if it is non-nil, else return the alternate
#define LIVE_ELSE(obj, alternate) \
({ id _obj = (obj); _obj ? _obj : (alternate); })


// type assertions, casting checks

#define ASSERT_KIND(obj, class_name) \
NSAssert2(IS_KIND((obj), class_name), \
@"object is not of class: %@; actual: %@", [class_name class], [(obj) class])

#define ASSERT_KIND_OR_NIL(obj, class_name) \
NSAssert2(!obj || IS_KIND((obj), class_name), \
@"non-nil object is not of class: %@; actual: %@", [class_name class], [(obj) class])


// cast with a run-time type kind assertion
#define CAST(class_name, ...) \
({ id _obj = (__VA_ARGS__); ASSERT_KIND_OR_NIL(_obj, class_name); (class_name*)_obj; })


// assertion for ENTERPRISE and DISTRIBUTION builds
#define CHECK(condition) \
if (!(condition)) [NSException raise:NSInternalInconsistencyException format:@"%s", #condition]

#define CHECK_KIND(obj, kind) CHECK(IS_KIND((obj), kind))

#define CHECK_KIND_OR_NIL(obj, kind) CHECK(IS_KIND_OR_NIL((obj), kind))

#define CHECK_MAIN_THREAD CHECK([NSThread isMainThread])


// shorthand to throw an exception in abstract base methods
#define OVERRIDE \
[NSException raise:NSInternalInconsistencyException format:@"%s: must override in subclass: %@", __FUNCTION__, [self class]]

#define NON_DESIGNATED_INIT(designated_name) \
[NSException raise:NSInternalInconsistencyException format:@"%s: non-designated initializer: instead use %@", __FUNCTION__, designated_name]; \
return nil


// catch clause for non-critical try blocks, where we want to ignore failure
#define CATCH_AND_LOG @catch (id exception) { FSLog(@"CAUGHT EXCEPTION: %s\n\%@", __FUNCTION__, exception); }

// declare a temporary qualified version of a variable
#define BLOCK_VAR(temp,     var) __block                __typeof__(var) temp = var
#define WEAK_VAR(temp,      var) __weak                 __typeof__(var) temp = var
#define UNSAFE_VAR(temp,    var) __unsafe_unretained    __typeof__(var) temp = var

#define BLOCK(var)  BLOCK_VAR(block_ ## var,    var)
#define WEAK(var)   WEAK_VAR(weak_ ## var,      var)
#define UNSAFE(var) UNSAFE_VAR(unsafe_ ## var,  var)


// assertions to make sure we are on the expected thread

#define ASSERT_MAIN_THREAD \
NSAssert1([NSThread isMainThread], @"%s: must be called from the main thread", __FUNCTION__)

#define ASSERT_NOT_MAIN_THREAD \
NSAssert1(![NSThread isMainThread], @"%s: must be called from a background thread", __FUNCTION__)


// HTTP 1.1 code classes
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


BOOL httpCodeIsOfClass(int httpCode, FSNHTTPCodeClass httpClass);


@interface NSDictionary (FSN)

// creat a url-encoded query string; used to generate either a GET query or url-encoded POST body
- (NSString *)urlQueryString;

@end



@interface NSData (FSN)

// convenience methods to ensure that top-level json objects are of the expected container type
- (id)dictionaryFromJSONWithError:(NSError **)error; // error pointer must not be nil
- (id)arrayFromJSONWithError:(NSError **)error; // error pointer must not be nil

- (NSString *)stringFromUTF8;

// create a printable string with utf8 data, converting certain bytes into mnemonic characters
- (NSString *)debugString;

@end


@interface NSDate (FSN)

+ (NSTimeInterval)posixTime;

@end

@interface NSArray (FSN)

- (NSString *)urlEncodedString;

@end

@interface NSString (FSN)

// create a string from utf8 data
+ (NSString *)withUTF8Data:(NSData *)data;

// percent-escaped string
- (NSString *)urlEncodedString;

- (NSData *)UTF8Data;

@end


@interface NSNumber (FSN)

- (NSString*)urlEncodedString; // just returns stringValue

@end


@interface NSURLResponse (FSN)

- (NSInteger)statusCode; // provide default implementation underneath NSHTTPURLResponse; returns -1

@end
