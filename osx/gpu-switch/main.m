//
//  main.m
//  gpu-switch
//
//  Created by malte on 09/05/16.
//  Copyright © 2016 Malte Bargholz. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "GSMux.h"

static void printUsage(void);

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSArray *arguments = [[NSProcessInfo processInfo] arguments];
        if(arguments.count > 2 || arguments.count == 1 || ([arguments[1] isEqualToString:@"-h"] || [arguments[1] isEqualToString:@"--help"])){
            printUsage();
            return -1;
        }
        else{
            [GSMux switcherOpen];
            
            NSString *mod = arguments[1];
            if([mod isEqualToString:@"-i"]){
                [GSMux setMode:GSSwitcherModeForceIntegrated];
            }
            else if([mod isEqualToString:@"-d"]){
                [GSMux setMode:GSSwitcherModeForceDiscrete];
            }
            else if([mod isEqualToString:@"-a"]){
                [GSMux setMode:GSSwitcherModeDynamicSwitching];
            }
            else{
                printUsage();
                [GSMux switcherClose];
                return -1;
            }
        }
        
    }
    [GSMux switcherClose];
    return 0;
}

static void printUsage(void){
    NSLog(@"gpu switch usage:\n\t-i\t switch to integrated\n\t-d\t switch to discrete\n\t -a\t enable auto switching\n\t-h/--help display this usage information.\n");
}
