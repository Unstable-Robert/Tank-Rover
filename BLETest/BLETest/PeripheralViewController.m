//
//  PeripheralViewController.m
//  BLETest
//
//  Created by Robert Mannuzza on 1/23/15.
//  Copyright (c) 2015 Robert Mannuzza. All rights reserved.
//

#import "PeripheralViewController.h"


@interface PeripheralViewController () {
    CBService *UART;
    CBCharacteristic *sendDataCharacteristic;
    CBCharacteristic *readDataCharacteristic;
    
    UInt8 leftMotorDir;
    UInt8 rightMotorDir;
    UInt8 leftMotorSpeed;
    UInt8 rightMotorSpeed;
    
    BOOL sendNewData;
    
    CGFloat moveToHeading;
}

@end
static NSString *TXUUID = @"6e400002-b5a3-f393-e0a9-e50e24dcca9e";
static NSString *RXUUID = @"6e400003-b5a3-f393-e0a9-e50e24dcca9e";
static NSString *UARTUUID = @"6e400001-b5a3-f393-e0a9-e50e24dcca9e";

@implementation PeripheralViewController
@synthesize toConnect, bleAdapter,leftJoyStick,rightJoyStick,leftTouch,rightTouch,compassView,btValues;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [self connectClick];
    NSLog(@"%@", self.toConnect.name);
    [self.bleAdapter setDelegate:self];
    leftJoyStick.height = 0.5;
    rightJoyStick.height = 0.5;
    compassView.angle = 0;
    moveToHeading = 0;
    [btValues setText:@"HI"];
    
    [NSTimer scheduledTimerWithTimeInterval:1.0
                                     target:self
                                   selector:@selector(myThreadMethod)
                                   userInfo:nil
                                    repeats:YES];
    [NSTimer scheduledTimerWithTimeInterval:0.001
                                     target:self
                                   selector:@selector(updateCompassHeading)
                                   userInfo:nil
                                    repeats:YES];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)setData:(BLEAdapter *)adapter selectedPeripheral:(CBPeripheral *)peripheral {
    self.bleAdapter = adapter;
    self.toConnect = peripheral;
}

- (void)myThreadMethod {
    if (sendNewData) {
        NSLog(@"sendNewData");
        [self updateHexValues];
        [self sendNewValues];
    } else {
        NSLog(@"stopMotors");
        [self stopMotors];
    }
}
- (void)updateCompassHeading {
    if ((moveToHeading > compassView.angle) && ((moveToHeading - compassView.angle) <= 180)) {
        compassView.angle = compassView.angle + 1;
        [compassView setNeedsDisplay];
    } else if ((moveToHeading > compassView.angle) && ((moveToHeading - compassView.angle) > 180)) {
        compassView.angle = compassView.angle - 1;
        [compassView setNeedsDisplay];
    } else if ((compassView.angle > moveToHeading) && ((compassView.angle - moveToHeading) <= 180)) {
        compassView.angle = compassView.angle - 1;
        [compassView setNeedsDisplay];
    } else if ((compassView.angle > moveToHeading) && ((compassView.angle - moveToHeading) > 180)) {
        compassView.angle = compassView.angle + 1;
        [compassView setNeedsDisplay];
    }
    if (compassView.angle < 0) {
        compassView.angle = compassView.angle + 360;
    }
    if (compassView.angle > 359) {
        compassView.angle = compassView.angle - 180;
    }
}
#pragma mark - Handling Touches

- (IBAction)leftTouchBegin:(id)sender {
    sendNewData = YES;
    CGPoint loc = [leftTouch locationInView:leftJoyStick];
    if (loc.y > 0) {
        CGFloat value = fabs(loc.y / leftJoyStick.frame.size.height);
        leftJoyStick.height = value;
        [leftJoyStick setNeedsDisplay];
        if ([self updateHexValues]) [self sendNewValues];
    }
}

- (IBAction)rightTouchBegins:(id)sender {
    sendNewData = YES;
    CGPoint loc = [rightTouch locationInView:rightJoyStick];
    if (loc.y > 0) {
        CGFloat value = fabs(loc.y / rightJoyStick.frame.size.height);
        rightJoyStick.height = value;
        [rightJoyStick setNeedsDisplay];
        if ([self updateHexValues]) [self sendNewValues];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    NSLog(@"touches ended");
    sendNewData = NO;
    leftJoyStick.height = 0.5;
    [leftJoyStick setNeedsDisplay];
    rightJoyStick.height = 0.5;
    [rightJoyStick setNeedsDisplay];
    
    [self stopMotors];
}

#pragma mark - Handling Hex Values 

- (BOOL)updateHexValues {
    BOOL update = false;
    if (leftMotorDir != leftJoyStick.getDirection) {
        leftMotorDir = leftJoyStick.getDirection;
        update = true;
    }
    if (leftMotorSpeed != leftJoyStick.getSpeed) {
        leftMotorSpeed = leftJoyStick.getSpeed;
        update = true;
    }
    if (rightMotorDir != rightJoyStick.getDirection) {
        rightMotorDir = rightJoyStick.getDirection;
        update = true;
    }
    if (rightMotorSpeed != rightJoyStick.getSpeed) {
        rightMotorSpeed = rightJoyStick.getSpeed;
        update = true;
    }
    return update;
}

#pragma mark - Bluetooth Communication
- (void)sendNewValues {
    char bytes[] = {leftMotorSpeed, rightMotorSpeed};
    NSData *data = [NSData dataWithBytes:bytes length:2];
    NSLog(@"%@", data);
    if (leftMotorSpeed != 0x00 || rightMotorSpeed != 0x00) {
     [self sendDataString:data];
    }
    
}

- (void)stopMotors {
    leftMotorSpeed = 0x96;
    rightMotorSpeed = 0x96;
    char bytes[] = {leftMotorSpeed,rightMotorSpeed};
    NSData *data = [NSData dataWithBytes:bytes length:2];
    [self sendDataString:data];
}
-(void) OnReadDataChanged:(BOOL)status : (CBCharacteristic *) characteristic {
    NSData *data = characteristic.value;
    UInt8 bytes[data.length];
    [data getBytes:bytes length:data.length];
    NSString *string = [[NSString alloc] initWithBytes:bytes length:data.length encoding:
            NSUTF8StringEncoding];
    string = [string stringByReplacingOccurrencesOfString:@"{" withString:@""];
    string = [string stringByReplacingOccurrencesOfString:@"}" withString:@""];
    NSArray *vals = [string componentsSeparatedByString:@","];
    CGFloat heading = [vals[0] integerValue];
    CGFloat pingSensor = [vals[1] integerValue];
    [btValues setText:[NSString stringWithFormat:@"H: %0.0f D: %0.0f", heading, pingSensor]];
    moveToHeading = heading;
}
- (void)sendDataString:(NSData *)tempData {
    [self.bleAdapter.activePeripheral writeValue:tempData
                               forCharacteristic:sendDataCharacteristic
                                            type:CBCharacteristicWriteWithoutResponse];
}

#pragma mark - connecting

- (void)connectClick {
    if (self.toConnect != nil) {
        [self.bleAdapter connectPeripheral:self.toConnect status:true];
    }
}

- (void)OnConnected:(BOOL)status {
    if (status) {
        [bleAdapter getAllServicesFromPeripheral:self.bleAdapter.activePeripheral];
    } else {
        [self connectionFailed];
    }
}

- (void)connectionFailed {
    [self displayMessage:@"Connection Lost"];
    [self.navigationController popViewControllerAnimated:YES];
}

- (void)OnDiscoverServices:(NSArray *)s {
    for (CBService *service in s) {
        if ([service.UUID isEqual:[CBUUID UUIDWithString:UARTUUID]]) {
            UART = service;
            [self.bleAdapter getAllCharacteristicsForService:self.bleAdapter.activePeripheral
                                                     service:UART];
        }
    }
}

- (void)OnDiscoverCharacteristics:(NSArray *)c {
    for (CBCharacteristic *character in c) {
        CBCharacteristicProperties properties = [character properties];
        /* // b r w1 w2 n i a e]*/
        
        NSString *props= [[NSString alloc] initWithFormat:@"B:%d R:%d w:%d W:%d N:%d I:%d A:%d E:%d",
                                     properties & CBCharacteristicPropertyBroadcast?1:0,
                                     properties & CBCharacteristicPropertyRead?1:0,
                                     properties & CBCharacteristicPropertyWriteWithoutResponse?1:0,
                                     properties & CBCharacteristicPropertyWrite?1:0,
                                     properties & CBCharacteristicPropertyNotify?1:0,
                                     properties & CBCharacteristicPropertyIndicate?1:0,
                                     properties & CBCharacteristicPropertyAuthenticatedSignedWrites?1:0,
                                     properties & CBCharacteristicPropertyExtendedProperties?1:0
                                     ];
        NSLog(@"%@", props);
        if ([character.UUID isEqual:[CBUUID UUIDWithString:TXUUID]]) {
            NSLog(@"Send");
            sendDataCharacteristic = character;
        }
        if ([character.UUID isEqual:[CBUUID UUIDWithString:RXUUID]]) {
            NSLog(@"Read");
            readDataCharacteristic = character;
            [bleAdapter.activePeripheral setNotifyValue:true forCharacteristic:readDataCharacteristic];
        }
    }
}

- (void)displayMessage:(NSString *)message {
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"DEBUG"
                                                    message:message delegate:self
                                          cancelButtonTitle:@"Ok"
                                          otherButtonTitles:nil];
    [alert show];
}


/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
