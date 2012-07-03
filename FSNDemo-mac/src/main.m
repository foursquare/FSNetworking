//
//  main.m
//  FSNDemo-mac
//
//  Created by George King on 7/16/12.
//  Copyright (c) 2012 Foursquare Labs, Inc. All rights reserved.
//  Permission to use this file is granted in FSNetworking/license.txt (apache v2).
//

#import <Foundation/Foundation.h>

#import "FSNConnection.h"


FSNConnection *makeConnection() {
    NSURL *url = [NSURL URLWithString:@"https://api.foursquare.com/v2/venues/search"];
    
    // to make a successful foursquare api request, add your own api credentials here.
    // for more information see: https://developer.foursquare.com/overview/auth
    
    NSDictionary *parameters =
    [NSDictionary dictionaryWithObjectsAndKeys:
     @"40.724209,-73.997162",   @"ll", // lat/lon
     @"00000000",               @"client_id",
     @"00000000",               @"client_secret",
     @"20120702",               @"v", // api version date
     nil];
    
    return [FSNConnection withUrl:url
                           method:FSNRequestMethodGET
                          headers:nil
                       parameters:parameters
                       parseBlock:^id(FSNConnection *c, NSError **error) {
                           NSDictionary *d = [c.responseData dictionaryFromJSONWithError:error];
                           if (!d) return nil;
                           
                           // example error handling.
                           // since the demo ships with invalid credentials,
                           // running it will demonstrate proper error handling.
                           // in the case of the 4sq api, the meta json in the response holds error strings,
                           // so we create the error based on that dictionary.
                           if (c.response.statusCode != 200) {
                               *error = [NSError errorWithDomain:@"FSAPIErrorDomain"
                                                            code:1
                                                        userInfo:[d objectForKey:@"meta"]];
                           }
                           
                           return d;
                       }
                  completionBlock:^(FSNConnection *c) {
                      NSLog(@"complete: %@\n\nerror: %@\n\nparseResult: %@\n", c, c.error, c.parseResult);
                      exit(BIT(c.error));
                  }
                    progressBlock:^(FSNConnection *c) {
                        NSLog(@"progress: %@: %.2f/%.2f", c, c.uploadProgress, c.downloadProgress);
                    }];
}


int main(int argc, const char * argv[])
{

    @autoreleasepool {
        
        FSNConnection *connection = makeConnection();
        NSLog(@"starting connection: %@", connection);
        [connection start];
        [[NSRunLoop mainRunLoop] run];
        
    }
    return 0;
}

