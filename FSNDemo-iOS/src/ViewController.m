//
//  ViewController.m
//  FSNDemo
//
//  Created by George King on 7/2/12.
//  Copyright 2012 Foursquare Labs, Inc. All rights reserved.
//  Permission to use this file is granted in FSNetworking/license.txt (apache v2).
//


#import "FSNConnection.h"
#import "ViewController.h"


@interface ViewController ()

@property (nonatomic) IBOutlet UIButton *cancelButton;
@property (nonatomic) IBOutlet UITextView *textView;
@property (nonatomic) FSNConnection *connection;

@end


@implementation ViewController

@synthesize
cancelButton = _cancelButton,
textView = _textView,
connection = _connection;


- (void)viewDidLoad {
    [super viewDidLoad];
    self.cancelButton.enabled = NO;
}


- (void)viewDidUnload {
    [super viewDidUnload];
}


- (FSNConnection *)makeConnection {
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
                      NSLog(@"complete: %@\n\nerror: %@\n\n", c, c.error);
                      self.textView.text = [c.parseResult description];
                      self.cancelButton.enabled = NO;
                      
                  }
                    progressBlock:^(FSNConnection *c) {
                        NSLog(@"progress: %@: %.2f/%.2f", c, c.uploadProgress, c.downloadProgress);
                    }];
}


- (IBAction)fetch {
    
    NSLog(@"fetching...");
    
    [self.connection cancel];
    
    self.connection = [self makeConnection];
    [self.connection start];
    self.cancelButton.enabled = YES;
}


- (IBAction)cancel {
    NSLog(@"canceling...");
    [self.connection cancel];
    self.connection = nil;
}


@end
