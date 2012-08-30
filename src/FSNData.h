//
//  FSNData.h
//  FSN
//
//  Created by George King on 10/11/11.
//  Copyright 2011-2012 Foursquare Labs, Inc. All rights reserved.
//  Permission to use this file is granted in FSNetworking/license.txt (apache v2).
//


// object encapsulating data for a POST parameter


// mime types; use underscores instead of camel case here to better map to the string values they represent.
typedef enum {
    MimeType_unknown = 0,
    MimeType_application_json,
    MimeType_image_jpeg,
} MimeType;


NSString *stringForMimeType(MimeType type);


@interface FSNData : NSObject

// NOTE: object property declarations in header are explicity 'strong' so that non-arc code may include the header.

@property (nonatomic, strong) NSData *data;
@property (nonatomic) MimeType mimeType;
@property (nonatomic, strong) NSString *fileName;

@property (nonatomic, strong, readonly) NSString *mimeTypeString;

- (id)initWithData:(NSData *)data mimeType:(MimeType)mimeType fileName:(NSString*)fileName;

+ (id)withData:(NSData *)data mimeType:(MimeType)mimeType fileName:(NSString*)fileName;

+ (id)withDataPath:(NSString*)path fileName:(NSString*)fileName;

#if TARGET_OS_IPHONE
+ (id)withImage:(UIImage*)image jpegQuality:(CGFloat)quality fileName:(NSString*)fileName;
#endif

@end
