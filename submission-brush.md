# Project 1: Brush

Please fill this out for Brush only. The project handout can be found [here](https://cs1230.graphics/projects/raster/1).

## Output Comparison
This is how you will demonstrate the functionality of your project.

For each of the rows below, follow the instructions to record a video. Please reference the [Screen Recording Guide](https://cs1230.graphics/docs/screen-recording/) for machine-specific guidance on how to do this.

Once you've recorded everything, navigate to this file in Github, click edit, and either select or drag-and-drop each of your videos into the correct location. This will upload them to GitHub (but not include them in the repo) and automatically embed them into this Markdown file by providing a link. Make sure to double-check that they all show up properly in the preview.

> Note: you can ignore the `student_outputs` folder in the repo for now. You will be using it in the next project, Filter.

We're **not** looking for your video to **exactly** match the expected output (i.e. draw the exact same shape). Just make sure to follow the instructions and verify that the recording fully demonstrates the functionality of that part of your project.

### Constant Brush
**Instructions:** Draw with the constant brush.

#### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/241d99b5-947a-407c-b454-410534520aad

#### Your Output


https://github.com/BrownCSCI1230/projects-raster-zguo47/assets/98437903/59f8e7b5-8a43-472d-8f13-0df8c7cc0c94


<!---
Paste your output on top of this comment!
-->



### Linear Brush
**Instructions:** Draw with the linear brush.

#### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/9706fa04-7710-441f-b292-ab010e04dec6

#### Your Output



https://github.com/BrownCSCI1230/projects-raster-zguo47/assets/98437903/10834b34-ec95-4c4e-82d8-d169ce4a2706




<!---
Paste your output on top of this comment!
-->



### Quadratic Brush
**Instructions:** Draw with the quadratic brush.

#### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/c5df5c09-bfe0-4c05-a56e-14609772d675

#### Your Output

https://github.com/BrownCSCI1230/projects-raster-zguo47/assets/98437903/680ba84f-c4be-4e6f-9c32-8a7c5c07f6ad

<!---
Paste your output on top of this comment!
-->



### Smudge Brush
**Instructions:** Draw some colors on the canvas and use the smudge brush to smear them together.

#### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/26440b63-2d1c-43fd-95f2-55b74ad3bbed

#### Your Output


https://github.com/BrownCSCI1230/projects-raster-zguo47/assets/98437903/eec912fe-8aad-4a95-8406-c6121e998a60


<!---
Paste your output on top of this comment!
-->



### Smudge Brush Change in Alpha
**Instructions:** Draw some colors on the canvas. Use the smudge brush with varying alpha levels (use at least three) and demonstrate that the brush still works the same way each time.

#### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/0b49c7d0-47ca-46d0-af72-48b831dfe7ea

#### Your Output


https://github.com/BrownCSCI1230/projects-raster-zguo47/assets/98437903/6203238f-491d-4d69-bc22-1df1fd8da99d


<!---
Paste your output on top of this comment!
-->



### Radius
**Instructions:** Use any brush with at least 3 different values for the radius.

#### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/6f619df6-80cd-4849-8831-6a5aade2a517

#### Your Output


https://github.com/BrownCSCI1230/projects-raster-zguo47/assets/98437903/2559f358-517e-4bd8-b5fd-c63f2dbcf46f


<!---
Paste your output on top of this comment!
-->



### Color
**Instructions:** Use any brush to draw red (255, 0, 0), green (0, 255, 0), and blue (0, 0, 255).

#### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/fd9578ca-e0af-433e-ac9e-b27db2ceebc9

#### Your Output


https://github.com/BrownCSCI1230/projects-raster-zguo47/assets/98437903/8e0b572b-9d1b-4c9a-a952-22190a8cc765


<!---
Paste your output on top of this comment!
-->



### Canvas Edge Behavior
**Instructions:** With any brush, click and draw on the canvas in a place where the mask intersects with the edge. Then, start drawing anywhere on the canvas and drag your mouse off of the edge.

#### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/f5344248-fa5f-4c33-b6df-ff0a45011c7a

#### Your Output


https://github.com/BrownCSCI1230/projects-raster-zguo47/assets/98437903/1a759d5b-f988-40a5-bf60-a3dcd545bd1c


<!---
Paste your output on top of this comment!
-->



### Alpha
**Instructions:** With the constant brush, draw a single dot of red (255, 0, 0) with an alpha of 255. Then, draw over it with a single dot of blue (0, 0, 255) with an alpha of 100. You should get a purpleish color.

#### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/b13d312d-d6d4-4375-aeaa-96174065443b

#### Your Output


https://github.com/BrownCSCI1230/projects-raster-zguo47/assets/98437903/0ac7cc7a-72cf-4d5b-815e-5e7544361be8



<!---
Paste your output on top of this comment!
-->



### Alpha of Zero
**Instructions:** Choose any brush and demonstrate that it will not draw if the alpha value is zero.

#### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/8e48777e-8196-401e-9af6-871abe712146

#### Your Output


https://github.com/BrownCSCI1230/projects-raster-zguo47/assets/98437903/fa96b34f-5727-4a1a-b48d-885b489aa5c5


<!---
Paste your output on top of this comment!
-->



## Design Choices
I create four separate initialize mask methods for each of the four types of brush, because it was easier to debug for me. 
I also separate the normal brush and smudge brush as two different paint methods. 
Apart from initializing the masks in settings change, I also initialize them in mouseDown, since they will
not be initialized if we don't change our settings and directly go painting. 

## Collaboration/References
Huge thanks to Tomas and Mehek for having to deal with my ill-designed code...

## Known Bugs
N/A

## Extra Credit


