//
//  AppDelegate.m
//  FSNDemo
//
//  Created by George King on 7/2/12.
//  Copyright 2012 Foursquare Labs, Inc. All rights reserved.
//  Permission to use this file is granted in FSNetworking/license.txt (apache v2).
//


#import "AppDelegate.h"

#import "ViewController.h"


@implementation AppDelegate

@synthesize
window = _window,
viewController = _viewController;


- (void)dealloc {
    [_window release];
    [_viewController release];
    [super dealloc];
}


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    self.viewController = [[[ViewController alloc] initWithNibName:@"ViewController" bundle:nil] autorelease];
    self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    return YES;
}


@end
