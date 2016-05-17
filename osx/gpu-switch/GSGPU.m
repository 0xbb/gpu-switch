//
//  GSGPU.m
//  gfxCardStatus
//
//  Created by Cody Krieger on 6/12/12.
//  Copyright (c) 2012 Cody Krieger. All rights reserved.
//
//  Caching removed by Malte Bargholz to make it more modular and independent of the original gfxCardStatus.

#import "GSGPU.h"
#import "GSMux.h"

#define kIOPCIDevice                "IOPCIDevice"
#define kIONameKey                  "IOName"
#define kDisplayKey                 "display"
#define kModelKey                   "model"

#define kIntelGPUPrefix             @"Intel"

#define kLegacyIntegratedGPUName    @"NVIDIA GeForce 9400M"
#define kLegacyDiscreteGPUName      @"NVIDIA GeForce 9600M GT"
#define k2010MacBookProDiscreteGPUName @"NVIDIA GeForce GT 330M"

#define kNukeItFromOrbitSwitchingOn2010MacBookPros @"nukeItFromOrbitSwitchingOn2010MacBookPros"





@implementation GSGPU

#pragma mark - GSGPU API

+ (NSArray *)getGPUNames
{
    NSMutableArray *cachedGPUs = [NSMutableArray array];
    
    // The IOPCIDevice class includes display adapters/GPUs.
    CFMutableDictionaryRef devices = IOServiceMatching(kIOPCIDevice);
    io_iterator_t entryIterator;
    
    if (IOServiceGetMatchingServices(kIOMasterPortDefault, devices, &entryIterator) == kIOReturnSuccess) {
        io_registry_entry_t device;
        
        while ((device = IOIteratorNext(entryIterator))) {
            CFMutableDictionaryRef serviceDictionary;
            
            if (IORegistryEntryCreateCFProperties(device, &serviceDictionary, kCFAllocatorDefault, kNilOptions) != kIOReturnSuccess) {
                // Couldn't get the properties for this service, so clean up and
                // continue.
                IOObjectRelease(device);
                continue;
            }
            
            const void *ioName = CFDictionaryGetValue(serviceDictionary, @kIONameKey);
            
            if (ioName) {
                // If we have an IOName, and its value is "display", then we've
                // got a "model" key, whose value is a CFDataRef that we can
                // convert into a string.
                if (CFGetTypeID(ioName) == CFStringGetTypeID() && CFStringCompare(ioName, CFSTR(kDisplayKey), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
                    const void *model = CFDictionaryGetValue(serviceDictionary, @kModelKey);
                    
                    NSString *gpuName = [[NSString alloc] initWithData:(__bridge NSData *)model 
                                                              encoding:NSASCIIStringEncoding];
                    
                    [cachedGPUs addObject:gpuName];
                }
            }
            
            CFRelease(serviceDictionary);
        }
    }
    
    return cachedGPUs;
}

+ (NSString *)integratedGPUName
{
    NSString *integratedGPUName;
    if ([self isLegacyMachine]) {
        integratedGPUName = kLegacyIntegratedGPUName;
    } else {
        NSArray *gpus = [self getGPUNames];
        
        for (NSString *gpu in gpus) {
            // Intel GPUs have always been the integrated ones in newer machines
            // so far.
            if ([gpu hasPrefix:kIntelGPUPrefix]) {
                integratedGPUName = gpu;
                break;
            }
        }
    }
    
    return integratedGPUName;
}

+ (NSString *)discreteGPUName
{
    NSString *discreteGPUName;
    
    if ([self isLegacyMachine]) {
        discreteGPUName = kLegacyDiscreteGPUName;
    } else {
        NSArray *gpus = [self getGPUNames];
        
        for (NSString *gpu in gpus) {
            // Check for the GPU name that *doesn't* start with Intel, so that
            // both AMD and NVIDIA GPUs get detected here.
            if (![gpu hasPrefix:kIntelGPUPrefix]) {
                discreteGPUName = gpu;
                break;
            }
        }
    }
    
    return discreteGPUName;
}

+ (BOOL)isLegacyMachine
{

    NSArray *gpuNames = [self getGPUNames];

    return [gpuNames containsObject:kLegacyIntegratedGPUName]
                        && [gpuNames containsObject:kLegacyDiscreteGPUName];

}

+ (BOOL)is2010MacBookPro
{
    return [[self getGPUNames] containsObject:k2010MacBookProDiscreteGPUName];
}
@end
