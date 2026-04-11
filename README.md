# PlaneCut
Color quantization/palette generation program.
  
# How to use
````
Enter the image paths as arguments.
Use -p to change palette size. (from 1 to 256 colors) (default is 256)
Use -dither 0/1 to enable dithering. (default is 0)
Use -bits RGB to change the number of bits per channel. (default is 888)
Use -outdir to change output directory.

PlaneCut test.png -p 256 -dither 1 -bits 565 -outdir C:/test/
````

# Algorithm  
1) Put all pixels (colors) from the image into one bucket.
2) Find a bucket with the largest distance from the average color to the colors inside bucket.  
3) Split this bucket into 2 different buckets.  
4) If the number of buckets is less than the required palette size, go to step 2.  
5) After producing a list of buckets, generate a palette from the bucket list, calculating the colors from the arithmetic mean of the colors in each bucket. 
  
List of colors in one bucket can be represented as a three-dimensional point cloud. [Orthogonal regression is used to find a plane to split point cloud in two.](https://zalo.github.io/blog/line-fitting/).
    
After constructing the plane, colors behind and in front of the plane will be splitted into two new buckets.
To select a bucket for splitting, it is necessary to find a bucket with the largest summ of distances from each point to the average bucket point. The "redmean" formula is used for color distance calculation.
  
# Examples

|Original|PlaneCut|Gimp|Photoshop|
|---|---|---|---|
|![Original](/examples/correct256.png)|![PlaneCut](/examples/correct256-out.png)|![Gimp](/examples/correct256-gimp.png)|![Photoshop](/examples/correct256-ps.png)|
|![Original](/examples/tex.png)|![PlaneCut](/examples/tex-out.png)|![Gimp](/examples/tex-gimp.png)|![Photoshop](/examples/tex-ps.png)|
|![Original](/examples/соака.png)|![PlaneCut](/examples/соака-out.png)|![Gimp](/examples/соака-gimp.png)|![Photoshop](/examples/соака-ps.png)|
