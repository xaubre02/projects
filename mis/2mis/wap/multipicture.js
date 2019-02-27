// global variables
var multiImgClass = "multiimage";
var maxImgWidth   = 160; // pixels
var maxImgHeight  = 90;  // pixels
var displayTime   = 500; // miliseconds

// auxiliary variables
var multiImageSpanIndex;
var imageIndex;

// wait until the DOM is loaded
document.addEventListener("DOMContentLoaded", function(event) {
  modifyHTML();
});

/**
 * Modify the HTML source code.
 */
function modifyHTML() {
  createGallery();
  modifyMultiImageSpans();
}

/**
 * Open the image gallery.
 */
function openGalery(imgIndex, spanIndex) {
  // update global variables
  multiImageSpanIndex = spanIndex;
  imageIndex = imgIndex;
  // select image and display the gallery
  selectImage(0);
  document.getElementById('mi_gallery').style.display = "block";
  return false;
}

/**
 * Close the image gallery.
 */
function closeGalery() {
  document.getElementById('mi_gallery').style.display = "none";
}

/**
 * Modify all multi-image spans in the document.
 */
function modifyMultiImageSpans() {
  // create an indicator of multiple images
  var mark = document.createElement("img");
  mark.src = "mark.png";
  mark.style.width      = "24px";
  mark.style.position   = "absolute";
  mark.style.top        = "10px";
  mark.style.right      = "10px";
  mark.style.zIndex     = "2";
  mark.style.transition = "visibility 0.1s, opacity 0.1s ease";
  
  var spans = document.getElementsByClassName(multiImgClass);
  for (var i = 0; i < spans.length; i++) {
    processMultiImageSpan(spans[i], i, mark);
  }
}

/**
 * Process the multi-image span. Display only the first image.
 */
function processMultiImageSpan(span, spanIndex, mark) {
  var markClone;
  // set the span style
  span.style.position = "relative";
  span.style.display  = "inline-block";
  span.style.margin   = "auto";
  
  // get all hrefs in the current span (<a> tag)
  var hrefs = span.getElementsByTagName("a");
  // if there are more than one image, mark the first one
  if (hrefs.length > 1) {
    markClone = mark.cloneNode();
    span.appendChild(markClone);
  }

  // calculate polygon params
  var angle = 360 / (hrefs.length - 1) * Math.PI / 180; // e.g. 5 images in total -> 1 is in the center -> 5 - 1, angle in radians
  var radius = Math.sqrt(maxImgWidth * maxImgWidth + maxImgHeight * maxImgHeight); // radius is set as diagonal of the maximum image size

  // modify all <a> tags
  for (var i = 0; i < hrefs.length; i++) {
    if (hrefs[i] === undefined) continue;
    
    // hide all images except the first one
    if (i > 0)
      hrefs[i].style.display = "none";

    // add onclick functionality
    hrefs[i].setAttribute("onclick", "return openGalery(" + i + ", " + spanIndex + ")");
    
    // modify style of an image
    var image = hrefs[i].getElementsByTagName("img")[0];
    if (image !== undefined) {
      image.style.maxWidth     = String(maxImgWidth) + "px";
      image.style.maxHeight    = String(maxImgHeight) + "px";
      image.style.padding      = "4px";
      image.style.borderRadius = "4px";
      image.style.border       = "1px solid #b3b3b3";
      image.style.transition   = "0.15s";
      image.style.zIndex       = "1";
      // mouse hover
      image.onmouseover = function() {
        this.style.boxShadow = "0 0 2px 1px rgba(100, 200, 255, 0.75)";
      }
      image.onmouseout = function() {
        this.style.boxShadow = "0 0 0 0 rgba(0, 0, 0, 0)";
      }
      
      // only the hidden images have different position
      if (i > 0) {
        // calculate the image position
        var coords = getVertexPos(i - 1, angle, radius);
        image.style.position = "absolute";
        image.style.top  = String(coords[0]) + "px";
        image.style.left = String(coords[1]) + "px";
      }
      // displayed preview timeout
      var timeout;
      image.onmouseenter = function() {
        // clear timeout when entering another image
        clearTimeout(timeout);
        // hide the mark
        markClone.style.opacity    = "0";
        markClone.style.visibility = "hidden";
        // add animation
        this.style.zIndex = "2"; // display on top of other images in case of lots of images
        this.style.transform  = "scale(1.05)";
        // display all hidden images
        for (var j = 1; j < hrefs.length; j++) {
          if (hrefs[j] === undefined) continue;
          hrefs[j].style.display = "block";
        }
      }
      image.onmouseleave = function() {
        // add animation
        this.style.zIndex = "1";
        this.style.transform  = "scale(0.95)";
        // hide all displayed images after a given time
        timeout = setTimeout(function() {
          // show the mark
          markClone.style.opacity    = "1";
          markClone.style.visibility = "visible";
          // hide all displayed images
          for (var j = 1; j < hrefs.length; j++) {
            if (hrefs[j] === undefined) continue;
            hrefs[j].style.display = "none";
          }
        }, displayTime);
      }
    }
  }
}

/**
 * Calculate the position of the vertex of the polygon.
 */
function getVertexPos(n, angle, radius) {
  // calculate the missing triangle sides
  var a = radius * Math.sin(n * angle); // |a| = |b| * sin(alpha)
  var c = radius * Math.cos(n * angle); // |c| = |b| * cos(alpha)
  return [Math.round(a), Math.round(c)];
}


/**
 * Create an image gallery.
 */
function createGallery() {
  var gallery = document.createElement("div");  // gallery element

  // gallery style
  gallery.id                    = "mi_gallery";
  gallery.style.display         = "none";
  gallery.style.position        = "fixed";
  gallery.style.zIndex          = "5";
  gallery.style.padding         = "0";
  gallery.style.top             = "0";
  gallery.style.left            = "0";
  gallery.style.width           = "100%";
  gallery.style.height          = "100%";
  gallery.style.overflow        = "auto";
  gallery.style.backgroundColor = "rgba(0,0,0,0.9)";
  gallery.style.textAlign       = "center";
  gallery.style.verticalAlign   = "middle";

  gallery.appendChild(createGalleryContent());
  document.body.appendChild(gallery);
}

/**
 * Create an image slot in the gallery.
 */
function createGalleryContent() {
  var content = document.createElement("div");  // gallery content
  var counter = document.createElement("div");  // image counter
  var image   = document.createElement("img");  // concrete image
  var close   = document.createElement("div");  // image close button
  
  // content
  content.style.position        = "relative";
  content.style.textAlign       = "center";
  content.style.margin          = "auto";
  content.style.display         = "inline-block";
  content.style.maxWidth        = "calc(100% - 60px)";
  content.style.maxHeight       = "calc(100% - 60px)";
  
  // image counter
  counter.id               = "gallery_cntr";
  counter.style.color      = "#f2f2f2";
  counter.style.fontSize   = "16px";
  counter.style.fontWeight = "bold";
  counter.style.padding    = "8px 12px";
  counter.style.position   = "absolute";
  counter.style.top        = "0";
  
  // image source
  image.id                 = "gallery_image";
  image.style.width        = "auto";
  image.style.maxWidth     = "100%";
  image.style.maxHeight    = "100%";

  // image close button
  close.className = "exit_button";
  close.setAttribute("onclick", "closeGalery()");
  close.innerHTML = "&times;";

  close.style.color      = "white";
  close.style.position   = "absolute";
  close.style.top        = "-15px";
  close.style.right      = "15px";
  close.style.fontSize   = "42px";
  close.style.fontWeight = "bold";
  close.style.cursor     = "pointer";
  close.style.transition = "0.4s ease";
  close.style.overflow   = "hidden";
  close.style.height     = "50px";

  close.onmouseover = function() {
    this.style.color = "#999";
  }
  close.onmouseout = function() {
    this.style.color = "white";
  }
  
  // next/prev button
  var next = document.createElement("a");
  next.style.cursor       = "pointer";
  next.style.position     = "absolute";
  next.style.top          = "calc(50% - 30px)";
  next.style.width        = "18px";
  next.style.padding      = "16px";
  //next.style.marginTop    = "-50px";
  next.style.color        = "white";
  next.style.fontWeight   = "bold";
  next.style.fontSize     = "20px";
  next.style.transition   = "0.4s ease";
  
  var prev = next.cloneNode(); // clone element "next" with its style

  next.innerHTML          = "&#10095;";
  next.style.borderRadius = "3px 0 0 3px";
  next.style.right        = "0";
  next.setAttribute("onclick", "selectImage(+1)");
  
  prev.innerHTML          = "&#10094;";
  prev.style.borderRadius = "0 3px 3px 0";
  prev.style.left         = "0";
  prev.setAttribute("onclick", "selectImage(-1)");
  // mouse hover - change opacity
  next.onmouseover = function() {
    this.style.backgroundColor = "rgba(0, 0, 0, 0.7)";
  }
  next.onmouseout = function() {
    this.style.backgroundColor = "rgba(0, 0, 0, 0)";
  }
  
  prev.onmouseover = function() {
    this.style.backgroundColor = "rgba(0, 0, 0, 0.7)";
  }
  prev.onmouseout = function() {
    this.style.backgroundColor = "rgba(0, 0, 0, 0)";
  }

  // append everything (create hiearchy)
  content.appendChild(image);
  content.appendChild(counter);
  content.appendChild(close);
  content.appendChild(next);
  content.appendChild(prev);
  
  return content;
}

/**
 * Select an image to be displayed in the gallery.
 */
function selectImage(offset) {
  span = document.getElementsByClassName(multiImgClass)[multiImageSpanIndex];
  aTags = span.getElementsByTagName("a");

  // update image index
  imageIndex += offset;
  if (imageIndex > aTags.length - 1) {imageIndex = 0;}
  else if (imageIndex < 0)           {imageIndex = aTags.length - 1;}
  
  var cntr  = document.getElementById("gallery_cntr");  // gallery image counter
  var image = document.getElementById("gallery_image"); // gallery image

  // link to the image full resolution
  image.src = aTags[imageIndex].href;
  cntr.textContent = (imageIndex + 1) + " / " + (aTags.length);
}