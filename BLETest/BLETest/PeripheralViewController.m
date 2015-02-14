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
}

@end
static NSString *TXUUID = @"6e400002-b5a3-f393-e0a9-e50e24dcca9e";
static NSString *RXUUID = @"6e400003-b5a3-f393-e0a9-e50e24dcca9e";
static NSString *UARTUUID = @"6e400001-b5a3-f393-e0a9-e50e24dcca9e";

@implementation PeripheralViewController
@synthesize toConnect, bleAdapter,leftJoyStick,rightJoyStick,leftTouch,rightTouch;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [self connectClick];
    NSLog(@"%@", self.toConnect.name);
    [self.bleAdapter setDelegate:self];
    leftJoyStick.height = 0.5;
    rightJoyStick.height = 0.5;
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)setData:(BLEAdapter *)adapter selectedPeripheral:(CBPeripheral *)peripheral {
    self.bleAdapter = adapter;
    self.toConnect = peripheral;
}

#pragma mark - Handling Touches

- (IBAction)leftTouchBegin:(id)sender {
    CGPoint loc = [leftTouch locationInView:leftJoyStick];
    if (loc.y > 0) {
        CGFloat value = fabsf(loc.y / leftJoyStick.frame.size.height);
        leftJoyStick.height = value;
        [leftJoyStick setNeedsDisplay];
        [self updateHexValues];
    }
}

- (IBAction)rightTouchBegins:(id)sender {
    CGPoint loc = [rightTouch locationInView:rightJoyStick];
    if (loc.y > 0) {
        CGFloat value = fabsf(loc.y / rightJoyStick.frame.size.height);
        rightJoyStick.height = value;
        [rightJoyStick setNeedsDisplay];
        [self updateHexValues];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    leftJoyStick.height = 0.5;
    [leftJoyStick setNeedsDisplay];
    rightJoyStick.height = 0.5;
    [rightJoyStick setNeedsDisplay];
    [self stopMotors];
}

#pragma mark - Handling Hex Values 

- (void)updateHexValues {
    if (leftMotorDir != leftJoyStick.getDirection) {
        leftMotorDir = leftJoyStick.getDirection;
    }
    if (leftMotorSpeed != leftJoyStick.getSpeed) {
        leftMotorSpeed = leftJoyStick.getSpeed;
        [self sendNewValues];
    }
    if (rightMotorDir != rightJoyStick.getDirection) {
        rightMotorDir = rightJoyStick.getDirection;
    }
    if (rightMotorSpeed != rightJoyStick.getSpeed) {
        rightMotorSpeed = rightJoyStick.getSpeed;
        [self sendNewValues];
    }

}

#pragma mark - Bluetooth Communication
- (void)sendNewValues {
    char bytes[] = {leftMotorDir,leftMotorSpeed,rightMotorDir,rightMotorSpeed};
    NSData *data = [NSData dataWithBytes:bytes length:4];
    NSLog(@"%@", data);
    if (leftMotorSpeed != 0x00 || rightMotorSpeed != 0x00) {
     [self sendDataString:data];
    }
    
}

- (void)stopMotors {
    char bytes[] = {0x00,0x00,0x00,0x00};
    NSData *data = [NSData dataWithBytes:bytes length:4];
    [self sendDataString:data];
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
        if ([character.UUID isEqual:[CBUUID UUIDWithString:TXUUID]]) {
            sendDataCharacteristic = character;
        }
        if ([character.UUID isEqual:[CBUUID UUIDWithString:RXUUID]]) {
            readDataCharacteristic = character;
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
