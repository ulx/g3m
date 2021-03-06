//
//  G3MViewController.m
//  G3MApp
//
//  Created by Mari Luz Mateo on 18/02/13.
//  Copyright (c) 2013 Igo Software SL. All rights reserved.
//

#import "G3MViewController.h"

#import <G3MiOSSDK/G3MWidget_iOS.h>
#import <G3MiOSSDK/G3MWidget.hpp>
#import <G3MiOSSDK/G3MBuilder_iOS.hpp>
#import <G3MiOSSDK/GInitializationTask.hpp>
#import <G3MiOSSDK/PlanetRendererBuilder.hpp>
#import <G3MiOSSDK/LayerBuilder.hpp>
#import <G3MiOSSDK/MarksRenderer.hpp>
#import <G3MiOSSDK/ShapesRenderer.hpp>
#import <G3MiOSSDK/MeshRenderer.hpp>
#import <G3MiOSSDK/MercatorTiledLayer.hpp>
#import <G3MiOSSDK/LayerTilesRenderParameters.hpp>
#import <G3MiOSSDK/MapQuestLayer.hpp>
#import <G3MiOSSDK/SingleBillElevationDataProvider.hpp>
#import "G3MToolbar.h"
#import "G3MAppUserData.hpp"
#import "G3MMarkUserData.hpp"
#import "G3MMeshRenderer.hpp"
#import "G3MMarksRenderer.hpp"
#import "G3MPlaneParseTask.hpp"
#import "G3MAppInitializationTask.hpp"
#import "G3MWebViewController.h"

@interface G3MViewController ()

@end

@implementation G3MViewController

@synthesize g3mWidget     = _g3mWidget;
@synthesize demoSelector  = _demoSelector;
@synthesize demoMenu      = _demoMenu;
@synthesize toolbar       = _toolbar;
@synthesize layerSwitcher = _layerSwitcher;

- (id)initWithNibName:(NSString *)nibNameOrNil
               bundle:(NSBundle *)nibBundleOrNil
{
  self = [super initWithNibName:nibNameOrNil
                         bundle:nibBundleOrNil];
  if (self) {
    // Custom initialization
  }
  return self;
}

- (void)viewDidLoad
{
  [super viewDidLoad];

  // Create a builder
  G3MBuilder_iOS builder(self.g3mWidget);


  const float verticalExaggeration = 6.0f;
  builder.getPlanetRendererBuilder()->setVerticalExaggeration(verticalExaggeration);

  ElevationDataProvider* elevationDataProvider = new SingleBillElevationDataProvider(URL("file:///full-earth-2048x1024.bil", false),
                                                                                     Sector::fullSphere(),
                                                                                     Vector2I(2048, 1024));
  builder.getPlanetRendererBuilder()->setElevationDataProvider(elevationDataProvider);


  LayerSet* layerSet = [self createLayerSet: true];
  builder.getPlanetRendererBuilder()->setLayerSet(layerSet);

  ShapesRenderer* shapeRenderer = new ShapesRenderer();
  shapeRenderer->setEnable(false);
  builder.addRenderer(shapeRenderer);

  MarksRenderer* marksRenderer = G3MMarksRenderer::createMarksRenderer(self);
  marksRenderer->setEnable(false);
  builder.addRenderer(marksRenderer);

  MeshRenderer* meshRenderer = G3MMeshRenderer::createMeshRenderer(builder.getPlanet());
  meshRenderer->setEnable(false);
  builder.addRenderer(meshRenderer);

  G3MAppUserData* userData = new G3MAppUserData(layerSet,
                                                shapeRenderer,
                                                marksRenderer,
                                                meshRenderer);
  userData->setSatelliteLayerEnabled(true);
//  userData->setLayerSet([self createLayerSet: userData->getSatelliteLayerEnabled()]);
//  userData->setMarksRenderer(marksRenderer);
//  userData->setShapeRenderer(shapeRenderer);
//  userData->setMeshRenderer(meshRenderer);

  // Setup the builder
  //  builder.getPlanetRendererBuilder()->setShowStatistics(true);
  builder.setInitializationTask(new G3MAppInitializationTask(self.g3mWidget), true);
  builder.setUserData(userData);

  // Initialize widget
  builder.initializeWidget();
  [self showSimpleGlob3];

  [self initDropDownMenu];
  [self initToolbar];
}

- (void)viewDidAppear:(BOOL)animated
{
  [super viewDidAppear:animated];

  // Let's get the show on the road!
  [self.g3mWidget startAnimation];
}

- (void)viewDidDisappear:(BOOL)animated
{
  // Stop the glob3 render
  [self.g3mWidget stopAnimation];

	[super viewDidDisappear:animated];
}

- (void)viewDidUnload
{
  self.g3mWidget     = nil;
  self.toolbar       = nil;
  self.layerSwitcher = nil;
  self.demoSelector  = nil;
  self.demoMenu      = nil;

  [super viewDidUnload];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
  // Return YES for supported orientations
  if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
  }
  else {
    return YES;
  }
}

- (void)didReceiveMemoryWarning
{
  [super didReceiveMemoryWarning];
  // Dispose of any resources that can be recreated.
}

- (LayerSet*) createLayerSet: (bool) satelliteLayerEnabled
{
  LayerSet* layers = LayerBuilder::createDefaultSatelliteImagery();
  // store satellite layers names
  satelliteLayersNames = LayerBuilder::getDefaultLayersNames();

  layers->addLayer(LayerBuilder::createOSMLayer(!satelliteLayerEnabled));


  MapQuestLayer* meteoriteBase = MapQuestLayer::newOSM(TimeInterval::fromDays(30));
  meteoriteBase->setEnable(false);
  layers->addLayer(meteoriteBase);

  std::vector<std::string> subdomains;
  subdomains.push_back("0.");
  subdomains.push_back("1.");
  subdomains.push_back("2.");
  subdomains.push_back("3.");

  MercatorTiledLayer* meteorites = new MercatorTiledLayer("CartoDB-meteoritessize",
                                                          "http://",
                                                          "tiles.cartocdn.com/osm2/tiles/meteoritessize",
                                                          subdomains,
                                                          "png",
                                                          TimeInterval::fromDays(90),
                                                          true,
                                                          Sector::fullSphere(),
                                                          2,
                                                          17,
                                                          NULL);
  meteorites->setEnable(false);
  layers->addLayer(meteorites);

  WMSLayer* csiro = new WMSLayer("g3m:mosaic-sst,g3m:mosaic-sla",
                                 URL("http://ooap-dev.it.csiro.au/geoserver/g3m/wms", false),
                                 WMS_1_1_0,
                                 Sector::fullSphere(),
                                 "image/png",
                                 "EPSG:900913",
                                 "",
                                 true,
                                 NULL,
                                 TimeInterval::fromDays(30),
                                 true,
                                 LayerTilesRenderParameters::createDefaultMercator(1, 19));
  csiro->setEnable(false);
  layers->addLayer(csiro);

  return layers;
}


- (void) resetWidget
{
  G3MAppUserData* g3mAppUserData = (G3MAppUserData*) self.g3mWidget.userData;
  g3mAppUserData->getMarksRenderer()->setEnable(false);
  g3mAppUserData->getShapeRenderer()->setEnable(false);
  g3mAppUserData->getMeshRenderer()->setEnable(false);

  [self.g3mWidget stopCameraAnimation];

  LayerSet* layerSet = g3mAppUserData->getLayerSet();
  for (int i = 0; i < layerSet->size(); i++) {
    layerSet->getLayer(i)->setEnable(false);
  }
}

- (void) setSatelliteLayerEnabled: (bool) enabled
{
  G3MAppUserData* g3mAppUserData = (G3MAppUserData*) self.g3mWidget.userData;
  LayerSet* layerSet = g3mAppUserData->getLayerSet();
  // satellite layers
  for (int i = 0; i < satelliteLayersNames.size(); i++) {
    layerSet->getLayerByName(satelliteLayersNames[i])->setEnable(enabled);
  }

  g3mAppUserData->setSatelliteLayerEnabled(enabled);
}

- (void) showSimpleGlob3
{
  [self setSatelliteLayerEnabled: true];

  [self.g3mWidget setAnimatedCameraPosition: Geodetic3D(Angle::fromDegrees(0),
                                                        Angle::fromDegrees(0),
                                                        25000000)
                               timeInterval: TimeInterval::fromSeconds(5)];
}

- (void) switchLayer
{
  G3MAppUserData* g3mAppUserData = (G3MAppUserData*) self.g3mWidget.userData;

  const bool satelliteLayerEnabled = g3mAppUserData->getSatelliteLayerEnabled();

  LayerSet* layerSet = g3mAppUserData->getLayerSet();
  // osm
  layerSet->getLayerByName("osm_auto:all")->setEnable(satelliteLayerEnabled);
  // satellite layers
  [self setSatelliteLayerEnabled: !satelliteLayerEnabled];

  if (satelliteLayerEnabled) {
    [[self layerSwitcher] setImage:[UIImage imageNamed:@"satellite-on-96x48.png"] forState:UIControlStateNormal];
  }
  else {
    [[self layerSwitcher] setImage:[UIImage imageNamed:@"map-on-96x48.png"] forState:UIControlStateNormal];
  }
}

- (void) showMarkersDemo
{
  [self setSatelliteLayerEnabled: true];
  G3MAppUserData* g3mAppUserData = (G3MAppUserData*) self.g3mWidget.userData;
  g3mAppUserData->getMarksRenderer()->setEnable(true);
  [self gotoPosition: Geodetic3D(Angle::fromDegrees(37.7658),
                                 Angle::fromDegrees(-122.4185),
                                 12000)];
}

- (void) showModelDemo
{
  [self setSatelliteLayerEnabled: true];
  G3MAppUserData* g3mAppUserData = (G3MAppUserData*) self.g3mWidget.userData;
  g3mAppUserData->getShapeRenderer()->setEnable(true);

  Shape* plane = g3mAppUserData->getPlane();
  plane->setPosition(new Geodetic3D(Angle::fromDegreesMinutesSeconds(38, 53, 42.24),
                                    Angle::fromDegreesMinutesSeconds(-77, 2, 10.92),
                                    10000));

  plane->setAnimatedPosition(TimeInterval::fromSeconds(26),
                             Geodetic3D(Angle::fromDegreesMinutesSeconds(38, 53, 42.24),
                                        Angle::fromDegreesMinutesSeconds(-78, 2, 10.92),
                                        10000),
                             true);

  const double fromDistance = 50000 * 1.5;
  const double toDistance   = 25000 * 1.5 / 2;

  const Angle fromAzimuth = Angle::fromDegrees(-90);
  const Angle toAzimuth   = Angle::fromDegrees(-90 + 360);

  const Angle fromAltitude = Angle::fromDegrees(90);
  const Angle toAltitude   = Angle::fromDegrees(15);

  plane->orbitCamera(TimeInterval::fromSeconds(20),
                     fromDistance, toDistance,
                     fromAzimuth,  toAzimuth,
                     fromAltitude, toAltitude);

  [self gotoPosition: Geodetic3D(Angle::fromDegreesMinutesSeconds(38, 53, 42.24),
                                 Angle::fromDegreesMinutesSeconds(-77, 2, 10.92),
                                 6000)];
}

- (void) showMeshDemo
{
  [self setSatelliteLayerEnabled: true];
  G3MAppUserData* g3mAppUserData = (G3MAppUserData*) self.g3mWidget.userData;
  g3mAppUserData->getMeshRenderer()->setEnable(true);
  [self gotoPosition: Geodetic3D(Angle::fromDegreesMinutesSeconds(38, 53, 42.24),
                                 Angle::fromDegreesMinutesSeconds(-77, 2, 10.92),
                                 6700000)];
}

- (void) showMeteoriteImpactsLayer
{
  G3MAppUserData* g3mAppUserData = (G3MAppUserData*) self.g3mWidget.userData;
  g3mAppUserData->setSatelliteLayerEnabled(false);
  LayerSet* layerSet = g3mAppUserData->getLayerSet();

  layerSet->getLayerByName("MapQuest-OSM")->setEnable(true);
  layerSet->getLayerByName("CartoDB-meteoritessize")->setEnable(true);
  layerSet->getLayerByName("g3m:mosaic-sst,g3m:mosaic-sla")->setEnable(true);


  [self.g3mWidget setAnimatedCameraPosition: Geodetic3D(Angle::fromDegrees(0),
                                                        Angle::fromDegrees(0),
                                                        25000000)
                               timeInterval: TimeInterval::fromSeconds(5)];
}

- (void) gotoPosition: (Geodetic3D) position
{
  [self.g3mWidget setAnimatedCameraPosition: position
                               timeInterval: TimeInterval::fromSeconds(5)];
}


- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
  NSString *title = [alertView buttonTitleAtIndex:buttonIndex];

  if([title isEqualToString:@"Learn more..."]) {
    G3MWebViewController *webView = [self.storyboard instantiateViewControllerWithIdentifier:@"G3MWebViewController"];
    //    [self presentModalViewController: webView
    //                            animated: YES];
    [self presentViewController:webView
                       animated:YES
                     completion:nil];
    [webView loadUrl: [NSURL URLWithString: urlMarkString]];
  }
}

- (void) initDropDownMenu
{
  // demoSelector: left align button text and style
  [self.demoSelector setContentHorizontalAlignment: UIControlContentHorizontalAlignmentLeft];
  [self.demoSelector setContentEdgeInsets: UIEdgeInsetsMake(0, 10, 0, 0)];
  UIImage *demoSelectorBg = [UIImage imageNamed: @"selector-background.png"];
  [self.demoSelector setBackgroundImage: demoSelectorBg forState: UIControlStateNormal];
  [self.demoSelector setBackgroundImage: demoSelectorBg forState: UIControlStateHighlighted];

  [self setDemoMenu: [[G3MUIDropDownMenu alloc] initWithIdentifier: @"demoMenu"]];

  NSMutableArray *demoNames = [[NSMutableArray alloc] initWithObjects:
                               @"Simple glob3",
                               @"Switch Layer",
                               @"Markers",
                               @"3D Model",
                               @"Point Mesh",
                               @"Meteorite Impacts",
                               nil];

  [self.demoMenu setDelegate: self];
  [self.demoMenu setMenuWidth: 200];
  [self.demoMenu setBackgroundColor: [UIColor darkGrayColor]];
  [self.demoMenu setBorderColor: [UIColor blackColor]];
  [self.demoMenu setTextColor: [UIColor lightGrayColor]];
  [self.demoMenu setTitleArray: demoNames];
  [self.demoMenu setValueArray: demoNames];
  [self.demoMenu makeMenu: self.demoSelector
               targetView: [self view]];
}

- (UIButton*) createToolbarButton: (NSString*) imageName
                            frame: (CGRect) frame
{
  UIButton* button = [UIButton buttonWithType: UIButtonTypeCustom];
  [button setTitle: @""
          forState: nil];
  [[button layer] setBorderWidth: 0];
  [button setImage: [UIImage imageNamed: imageName]
          forState: UIControlStateNormal];
  [button setFrame: frame];

  return button;
}

- (void) initToolbar
{
  [self setToolbar: [[G3MToolbar alloc] init]];
  [[self view] addSubview: [self toolbar]];

  // layerSwitcher
  [self setLayerSwitcher: [self createToolbarButton: @"satellite-on-96x48.png"
                                              frame: CGRectMake(10.0, 10.0, 96.0, 48.0)]];
  [[self layerSwitcher] addTarget: self
                           action: @selector(switchLayer)
                 forControlEvents: UIControlEventTouchUpInside];
}

- (void) updateToolbar: (NSString*) option
{
  [[self toolbar] clear];
  if ([option isEqual: @"Switch Layer"]) {
    [[self toolbar] addSubview: [self layerSwitcher]];
    [[self toolbar] setVisible: TRUE];
  }
  else {
    [[self toolbar] setVisible: FALSE];
  }
}

- (void) DropDownMenuDidChange: (NSString *) identifier
                              : (NSString *) returnValue
{
  if ([identifier isEqual: @"demoMenu"]) {
    [self resetWidget];
    [self updateToolbar: returnValue];
    [self.demoSelector setTitle: returnValue
                       forState: nil];

    if ([returnValue isEqual: @"Simple glob3"]) {
      [self showSimpleGlob3];
    }
    else if ([returnValue isEqual: @"Switch Layer"]) {
      [self switchLayer];
    }
    else if ([returnValue isEqual: @"Markers"]) {
      [self showMarkersDemo];
    }
    else if ([returnValue isEqual: @"3D Model"]) {
      [self showModelDemo];
    }
    else if ([returnValue isEqual: @"Point Mesh"]) {
      [self showMeshDemo];
    }
    else if ([returnValue isEqualToString: @"Meteorite Impacts"]) {
      [self showMeteoriteImpactsLayer];
    }
  }
}

@end

