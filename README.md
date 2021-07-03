# PlaneCut
Color quantization/palette generation program for png images.

The program uses my algorithm for indexing colors inspired by bsp and median cut. Maybe similar algorithms already exist, but I didn't look for information about it.
  
# How to use:  
````
Enter the paths to the images as arguments.
Use -p to change palette size. (from 1 to 256 colors) (default is 256)
Use -dither 0/1 to enable dithering. (default is 0)
Use -bits RGB to change the number of bits per channel. (default is 888)
Use -outdir to change output directory.

PlaneCut test.png -palette 256 -dither 1 -bits 565 -outdir C:/test/
````

# Algorithm  
1) Put all the colors in one bucket  
2) Find the bucket with the largest distance from the average color to the colors in it  
3) Divide this bucket into 2 different ones  
4) If the number of buckets is less than the required size of the palette, then go to step 2  
5) After making a list of buckets, convert them to a palette, getting the color from the arithmetic mean of the colors in the bucket  
  
List of colors in the bucket can be represented as a three-dimensional point cloud. Having such an array of points, I determine the point through which the section plane should pass in order to divide the point cloud into 2 buckets, this point will be the arithmetic mean of all the points in the bucket. It remains to determine the direction of the section plane, I use the algorithm from here for this zalo.github.io/blog/line-fitting/
    
After constructing the section plane, it is necessary to determine whether the points are located behind or in front of the section plane using the dot product, thus dividing the bucket into 2 parts.
To select a bucket for dividing, it is necessary to select a bucket whose sum of distances from the center point to the points located in the bucket is largest. The "redmean" formula is chosen for calculating distances, since it is easy to implement in code, and the image quality is higher than when using the Euclidean distance formula.
  
# Examples

|Original|PlaneCut|Gimp|Photoshop|
|---|---|---|---|
|![Original](/examples/correct256.png)|![PlaneCut](/examples/correct256-out.png)|![Gimp](/examples/correct256-gimp.png)|![Photoshop](/examples/correct256-ps.png)|
|![Original](/examples/tex.png)|![PlaneCut](/examples/tex-out.png)|![Gimp](/examples/tex-gimp.png)|![Photoshop](/examples/tex-ps.png)|
|![Original](/examples/соака.png)|![PlaneCut](/examples/соака-out.png)|![Gimp](/examples/соака-gimp.png)|![Photoshop](/examples/соака-ps.png)|
