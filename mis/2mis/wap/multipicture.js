// global variables
var multiImageClass = "multiimage";
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
  var spans = document.getElementsByClassName(multiImageClass);
  for (var i = 0; i < spans.length; i++) {
    processMultiImageSpan(spans[i], i);
  }
}

/**
 * Process the multi-image span. Display only the first image.
 */
function processMultiImageSpan(span, spanIndex) {
  // get all hrefs in the current span (<a> tag)
  var hrefs = span.getElementsByTagName("a");
  for (var i = 0; i < hrefs.length; i++) {
    if (hrefs[i] === undefined) continue
    
    // hide all images except the first one
    if (i > 0)
      hrefs[i].style.display = "none";

    // add onclick functionality
    hrefs[i].setAttribute("onclick", "return openGalery(" + i + ", " + spanIndex + ")");
    
    // modify style of all images
    var image = hrefs[i].getElementsByTagName("img")[0];
    if (image !== undefined) {
      image.style.width        = "200px";
      image.style.padding      = "4px";
      image.style.borderRadius = "4px";
      image.style.border       = "1px solid #b3b3b3";
      // mouse hover
      image.onmouseover = function() {
        this.style.boxShadow = "0 0 2px 1px rgba(100, 200, 255, 0.75)";
      }
      image.onmouseout = function() {
        this.style.boxShadow = "0 0 0 0 rgba(0, 0, 0, 0)";
      }
    }
  }
}

/**
 * Create an image gallery.
 */
function createGallery() {
	var gallery = document.createElement("div");  // gallery element

	// gallery
  gallery.id                    = "mi_gallery";
	gallery.style.display         = "none";
	gallery.style.position        = "fixed";
	gallery.style.zIndex          = "1";
	gallery.style.padding         = "25px 25px";
	gallery.style.top             = "0";
	gallery.style.left            = "0";
	gallery.style.width           = "100%";
	gallery.style.height          = "100%";
	gallery.style.overflow        = "auto";
	gallery.style.backgroundColor = "rgba(0,0,0,0.85)";
  gallery.style.textAlign       = "center";

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
  //content.style.backgroundColor = "rgba(0,0,0,0.0)";
	content.style.position        = "relative";
  content.style.textAlign       = "center";
	content.style.margin          = "auto";
  content.style.display         = "inline-block";
  content.style.maxWidth        = "75%";
  
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
	image.style.border       = "none";
	image.style.borderRadius = "0";
	image.style.padding      = "0";
  image.style.margin       = "auto";
  image.style.display      = "block";
  image.style.position     = "relative";

	image.onmouseover = function() {
		this.style.boxShadow = "none";
	}

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
	next.style.top          = "50%";
	next.style.width        = "18px";
	next.style.padding      = "16px";
	next.style.marginTop    = "-50px";
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
	// mouse hover
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
  span = document.getElementsByClassName(multiImageClass)[multiImageSpanIndex];
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