# POPSCL
Printed Open-Source Product Supply Counting Line -
This repository contains all the STL files needed to recreate Lunchbox Electronics Fulfillment Line POPSCL
In addition to the STL files:
STL files for the convayor belt pullies can be found here:
  http://www.andymark.com/product-p/am-2234.htm
STL files for the screws and nuts can be found here:
  http://www.mcmaster.com

All parts were printed on a Lulzbot Taz 5 with PLA or N-Gen filament

POPSCL was created to aid in the counting and packaging of Build Upons by Lunchbox Electronics. Instead of 
counting out a certain number of bricks and bagging them for hours, POPSCL will take bricks from a large
hopper and put them one by one onto a conveyor belt to be counted. These bricks are then dropped into a 
bucket to be dumped into a bag by the operator. 

The basic workflow of POPSCL is:
  1. A bunch of bricks get dumped into the hopper
  2. The cleated belt runs through the hopper picking bricks up one by one
  3. The cleated belt drops the individual bricks onto the prefeeder which is the transition to the conveyor belt.
  4. Once on the conveyor belt the bricks pass through a IR gate (this break of the IR beam is registered and each
brick is counted as it goes by)
  5. The bricks then fall off the conveyor belt in a bucket on the carousel
  6. When the determined number of bricks per bag is reached all motors stop and the carousel rotates to fill an
empty bucket with more bricks
  7. This process continues till there are no more bricks to bag


# Bill of Materials:
- All supplied 3D printed STL files
- Additional non-printed hardwear (per assembly)
  - Hopper
    - 2 #2-56 by 1/4" socket cap hex screws
    - 2 1.125" bearing (http://bit.ly/1Tkejrw purchased from Fastenal)
  - Prefeeder
    - 2 #2-56 by 1/4" socket cap hex screws
    - 1 1.125" bearing (http://bit.ly/1Tkejrw purchased from Fastenal)
  - Cleated Belt
    - L series timing belt, 24.8" outer circle (http://www.mcmaster.com/#6484k704/=128nk5q)
  - Conveyor Belt
    - 2 M5 by 1" machine screws
    - 2 M4 by 1" machine screws
    - 2 DC motors (sourced from Hummingbird Duo kit, http://www.hummingbirdkit.com/learning/duo-datasheet#fMotor)
    - STL files for 2 pullies from AndyMark (http://www.andymark.com/product-p/am-2234.htm)
    - HTD timing belt, 1,000mm outer circle (http://www.mcmaster.com/#7939k31/=128nldv)
    - IR emitter and IR detector (https://www.sparkfun.com/products/241)
    - 3 1.125" bearing (http://bit.ly/1Tkejrw purchased from Fastenal)
  - Carousel
    - 3 #4-40 by 1/4" machine screws
    - 3 #4-40 threaded inserts for plastic (http://www.mcmaster.com/#93365a120/=12964yt)
    - Stepper motor (https://www.sparkfun.com/products/9238)
    - Photocell (https://www.sparkfun.com/products/9088)
    - Half of a lulzbot filament reel
- Arduino
- Sparkfun Easydriver board
- Sparkfun Power Driver Shield Kit



