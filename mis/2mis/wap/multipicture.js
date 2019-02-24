// wait until the DOM is loaded
document.addEventListener("DOMContentLoaded", function(event) {
    adjustDocumentMultiImages('multiimage');
	createPreview();
});

/**
 * Open the image preview.
 */
function openPreview() {
	document.getElementById('mi_preview').style.display = "block";
	return false;
}

/**
 * Close the image preview.
 */
function closePreview() {
	document.getElementById('mi_preview').style.display = "none";
}

/**
 * Adjust the document multi-images.
 */
function adjustDocumentMultiImages(className) {
	var spans = document.getElementsByClassName(className);
	for (var i = 0; i < spans.length; i++) {
		processMultiImageSpan(spans[i]);
	}
}

/**
 * Process the multi-image span. Display only the first image.
 */
function processMultiImageSpan(span) {
	// get all images in the current span
	var images = span.getElementsByTagName("img");
	// hide all images except the first one
	for (var i = 1; i < images.length; i++) {
		images[i].style.display = "none";
	}
}

function changeImage() {
	var img = document.getElementById("large_image");
	
	img.src = "https://www.sciencemag.org/sites/default/files/styles/article_main_large/public/images/cc_Giraffes_16x9.jpg?itok=dKmuVKO6";
}

/**
 * Create an image preview.
 */
function createPreview() {
	var preview = document.createElement("div");  // preview element
	var content = document.createElement("div");  // preview content
	var close   = document.createElement("span"); // preview close button
	// identificators
	preview.id        = "mi_preview";
	preview.className = "preview";
	content.className = "preview_content";

	// preview style
	preview.style.display = "none";
	preview.style.position = "fixed";
	preview.style.zIndex = "1";
	preview.style.paddingTop = "100px";
	preview.style.top = "0";
	preview.style.left = "0";
	preview.style.width = "100%";
	preview.style.height = "100%";
	preview.style.overflow = "auto";
	preview.style.backgroundColor = "black";
	
	// close button
	close.className = "exit_button";
	close.setAttribute("onclick", "closePreview()");
	close.innerHTML = "&times;";
	
	close.style.color = "white";
	close.style.position = "absolute";
	close.style.top = "10px";
	close.style.right = "40px";
	close.style.fontSize = "50px";
	close.style.fontWeight = "bold";
	close.style.cursor = "pointer";
	close.style.transition = "0.4s ease";
	// mouse movement
	close.onmouseover = function() {
		this.style.color = "#999";
	}
	close.onmouseout = function() {
		this.style.color = "white";
	}

	
	// content
	content.style.position = "relative";
	content.style.backgroundColor = "#fefefe";
	content.style.margin = "auto";
	content.style.padding = "0";
	content.style.width = "90%";
	content.style.maxWidth = "1200px";
	
	var next = document.createElement("a"); // next image button
	next.innerHTML = "&#10095;";
	next.style.cursor = "pointer";
	next.style.position = "absolute";
	next.style.right = "0";
	next.style.top = "50%";
	next.style.width = "18px";
	next.style.padding = "16px";
	next.style.marginTop = "-50px";
	next.style.color = "white";
	next.style.fontWeight = "bold";
	next.style.fontSize = "20px";
	next.style.transition = "0.4s ease";
	next.style.borderRadius = "3px 0 0 3px";
	/*next.style.userSelect = "";
	next.style. = "";
	next.style. = "";*/
	
	
	var prev = next.cloneNode(); // previous image button
	prev.innerHTML = "&#10094;";
	prev.style.borderRadius = "0 3px 3px 0";
	prev.style.left = "0";
	
	next.setAttribute("onclick", "changeImage()"); // TODO
	prev.setAttribute("onclick", "closePreview()"); // TODO
	// not working
	// mouse movement
	
	// image caption
	var caption = document.createElement("div");   // image caption (container)
	var captionText = document.createElement("p"); // image caption
	// caption container
	caption.style.textAlign       = "center";
	caption.style.backgroundColor = "black";
	caption.style.padding         = "2px 16px";
	caption.style.color           = "white";
	// caption text
	captionText.id = "image_caption";
	caption.appendChild(captionText);
	
	//append everything to content
	content.appendChild(createImageSlot());
	content.appendChild(next);
	content.appendChild(prev);
	content.appendChild(caption);
		
	console.log(content.innerHTML);
	
	// append the rest
	preview.appendChild(close);
	preview.appendChild(content);
	document.body.appendChild(preview);
	
	var captionText = document.getElementById("image_caption");
	captionText.innerHTML = "Panda pičo";
	
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
}

/**
 * Create an image slot.
 */
function createImageSlot() {
	var slot    = document.createElement("div");  // image slot
	var counter = document.createElement("div");  // image counter
	var image   = document.createElement("img");  // some concrete image
	
	//slot.style.display = "none";
	
	// image counter style
	counter.textContent      = "1 / 1";
	counter.style.color      = "#f2f2f2";
	counter.style.fontSize   = "16px";
	counter.style.fontWeight = "bold";
	counter.style.padding    = "8px 12px";
	counter.style.position   = "absolute";
	counter.style.top        = "0";
	
	// image source
	image.id = "large_image";
	image.src = "http://m.wsj.net/video/20160906/20160906panda/20160906panda_1280x720.jpg";
	image.style.width        = "100%";
	image.style.border       = "none";
	image.style.borderRadius = "0";
	image.style.padding      = "0";
	// mouse hover
	image.onmouseover = function() {
		this.style.boxShadow = "none";
	}
	
	
	slot.appendChild(counter);
	slot.appendChild(image);
	
	return slot;
}