//
//  BLEAdapter.h
//  BLEServiceBrowser
//
//  Created by Muhammad on 3/25/13.
//  Main Wrapper class for bluetooth low energy functionality.
//  This class encapsulates all function calls and delegate methods needed to interact with the bluetooth SDK.
//  Copyright (c) 2013 Bluetooth SIG. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import <CoreBluetooth/CBService.h>
//#import "constants.h"

//#import "DeviceViewController.h"
//#import "ServiceViewController.h"
//#import "CharViewController.h"


// Delegate methods decalration.
@protocol BLEAdapterDelegate<NSObject>
@optional
-(void) OnReadDataChanged:(BOOL)status : (CBCharacteristic *) characteristic;
-(void) OnWriteDataChanged:(BOOL)status;
-(void) OnDiscoverCharacteristics:(NSArray *)c;
-(void) OnDiscoverServices:(NSArray *)s;
-(void) OnConnected:(BOOL)status;
@end

@interface BLEAdapter : NSObject <CBCentralManagerDelegate, CBPeripheralDelegate>
{
    id<BLEAdapterDelegate> delegate;
}

+(BLEAdapter*) sharedBLEAdapter;

@property (retain) id delegate;

@property (strong, nonatomic) NSMutableArray *peripherals;
@property (strong, nonatomic) CBCentralManager *CM;
@property (strong, nonatomic) CBPeripheral *activePeripheral;


-(void) writeValue:(int)serviceUUID characteristicUUID:(int)characteristicUUID  p:(CBPeripheral *)p data:(NSData *)data;
-(void) readValue: (int)serviceUUID characteristicUUID:(int)characteristicUUID  p:(CBPeripheral *)p;
-(void) notification:(int)serviceUUID characteristicUUID:(int)characteristicUUID  p:(CBPeripheral *)p on:(BOOL)on;

-(UInt16) swap:(UInt16) s;
-(int) controlSetup:(int) s;
//-(int) findBLEPeripherals:(int) timeout;
-(int) findBLEPeripherals:(int) timeout serviceUUID:(NSString*)serviceUUID;
-(const char *) centralManagerStateToString:(int)state;
-(void) scanTimer:(NSTimer *)timer;
-(void) printKnownPeripherals;
-(void) printPeripheralInfo:(CBPeripheral*)peripheral;
-(void) connectPeripheral:(CBPeripheral *)peripheral status:(BOOL)status;

-(void) getAllCharacteristicsForService:(CBPeripheral *)p service:(CBService *)s;
-(void) getAllServicesFromPeripheral:(CBPeripheral *)p;
-(void) getAllCharacteristicsFromPeripheral:(CBPeripheral *)p;
-(CBService *) findServiceFromUUID:(CBUUID *)UUID p:(CBPeripheral *)p;
-(CBCharacteristic *) findCharacteristicFromUUID:(CBUUID *)UUID service:(CBService*)service;
-(NSString *) CBUUIDToNSString:(CBUUID *) UUID;
-(const char *) UUIDToString:(CFUUIDRef) UUID;
-(const char *) CBUUIDToString:(CBUUID *) UUID;
-(int) compareCBUUID:(CBUUID *) UUID1 UUID2:(CBUUID *)UUID2;
-(NSData *)dataFromHexString:(NSString *)string;
-(int) compareCBUUIDToInt:(CBUUID *) UUID1 UUID2:(UInt16)UUID2;
-(UInt16) CBUUIDToInt:(CBUUID *) UUID;
-(int) UUIDSAreEqual:(CFUUIDRef)u1 u2:(CFUUIDRef)u2;

-(NSString *)GetServiceName:(CBUUID *)UUID;


@end
