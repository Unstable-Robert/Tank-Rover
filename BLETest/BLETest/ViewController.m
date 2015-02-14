//
//  ViewController.m
//  BLETest
//
//  Created by Robert Mannuzza on 1/9/15.
//  Copyright (c) 2015 Robert Mannuzza. All rights reserved.
//

#import "ViewController.h"
#import "PeripheralViewController.h"

@interface ViewController ()

@end

@implementation ViewController
@synthesize bleWrapper,tbl_data,selected;
NSTimer *scannerTimer;
static NSString * const kServiceUUID = @"6E400001-B5A3-F393-E0A9-E50E24DCCA9E";

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    UIRefreshControl *scanRefresh = [[UIRefreshControl alloc] init];
    [scanRefresh addTarget:self action:@selector(scan_click:) forControlEvents:UIControlEventValueChanged];
    [self.tbl_data addSubview:scanRefresh];
    
    self.bleWrapper = [BLEAdapter sharedBLEAdapter];
    [self.bleWrapper controlSetup:1];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
#pragma mark - TableView Data

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [[self.bleWrapper peripherals] count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *CellIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    
    CBPeripheral *temp = (CBPeripheral *)[self.bleWrapper. peripherals objectAtIndex:indexPath.row];
    cell.textLabel.text = temp.name;

    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    CBPeripheral *temp = (CBPeripheral *)[self.bleWrapper.peripherals objectAtIndex:indexPath.row];
    self.selected = temp;
    [self performSegueWithIdentifier:@"ScanToPeripheral" sender:self];
}


#pragma mark - Bluetooth Scaning

- (void)scan_click:(UIRefreshControl *)refreshControl {
    [self.bleWrapper findBLEPeripherals:10 serviceUUID:kServiceUUID];
    [UIApplication sharedApplication].networkActivityIndicatorVisible = YES;
    [NSTimer scheduledTimerWithTimeInterval:(float)2.0
                                     target:self
                                   selector:@selector(connectionTimer:)
                                   userInfo:nil
                                    repeats:NO];
    [refreshControl endRefreshing];
    
}

- (void)connectionTimer:(NSTimer *)timer {
    [UIApplication sharedApplication].networkActivityIndicatorVisible = NO;
    if (self.bleWrapper.peripherals > 0) {
        [self.tbl_data reloadData];
    } else {
        
    }
}

#pragma mark - Segue handling

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    if ([segue.identifier isEqualToString:@"ScanToPeripheral"]) {
        PeripheralViewController *destViewController = segue.destinationViewController;
        [destViewController setData:self.bleWrapper selectedPeripheral:self.selected];
    }
}
@end
