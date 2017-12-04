var last_submenu = -1;
var last_subcat = -1;
var last_cat = -1;
var caption = "";
var subcaption = "";

// Cookies
function set_cookies()
{
	document.cookie = "last_submenu=" + last_submenu;
	document.cookie = "last_subcat=" + last_subcat;
	document.cookie = "last_cat=" + last_cat;
	document.cookie = "caption=" + caption;
	document.cookie = "subcaption=" + subcaption;
}

function get_cookies()
{
	var list = document.cookie.split(';');
	
	for (var i = 0; i < list.length; i++)
	{
		var item = list[i].split("=");
		if (item[0] == "last_submenu")
		{
			last_submenu = item[1];
		}
		else if (item[0] == " last_subcat")
		{
			last_subcat = item[1];
		}
		else if (item[0] == " last_cat")
		{
			last_cat = item[1];
		}
		else if (item[0] == " caption")
		{
			caption = item[1];
		}
		else if (item[0] == " subcaption")
		{
			subcaption = item[1];
		}
	}
}

function load_from_cookies()
{
	get_cookies();
	select_category_catalog(-1);
	show_submenu(-1);
	select_subcategory(-1);
}

// Zobrazení nadpisu
function show_caption()
{
	var cap = document.getElementById("caption");
	cap.innerHTML = caption;
}

function show_subcaption()
{
	var subcap = document.getElementById("subcaption");
	subcap.innerHTML = subcaption;
}

// Selekce kategorie z jine stranky nez z katalogu
function select_category(id, cat)
{
	last_cat = id;
	last_submenu = id;
	set_cookies();
	window.location.assign('catalog.php?hlavniKategorie=' + cat);
}

// Selekce kategorie
function select_category_catalog(id)
{
	get_cookies();
	var category = document.getElementsByClassName("menu_item");
	// page load / refresh
	if(id == -1)
	{
		// nothing selected
		if (last_cat == -1)
		{	
			return;
		}
		else
		{
			category[last_cat].className += " selected";
			caption = category[last_cat].text;
			show_caption();
			return;
		}
	}
	
	if (last_cat != -1)
	{
		category[last_cat].className = category[last_cat].className.replace(" selected", "");
	}
	category[id].className += " selected";
	caption = category[id].text;
	show_caption();
	show_submenu(id);
	// Odoznačení poslední položky
	select_subcategory(-2);
	
	last_cat = id;
	set_cookies();
}

// Zobrazení submenu
function show_submenu(id)
{
	get_cookies();
	var submenu = document.getElementsByClassName("submenu");
	// page load / refresh
	if(id == -1)
	{
		// nothing selected
		if (last_submenu == -1)
		{	
			return;
		}
		else
		{
			submenu[last_submenu].className += " show";
			return;
		}
	}
	
	if( last_submenu != -1)
	{
		submenu[last_submenu].className = submenu[last_submenu].className.replace(" show", "");
	}
	
	submenu[id].className += " show";
	last_submenu = id;
	set_cookies();
}

// Označení položky
function select_subcategory(id)
{
	get_cookies();
	var subcategory = document.getElementsByClassName("submenu_item");
	// page load / refresh
	if(id == -1)
	{
		// nothing selected
		if (last_subcat == -1)
		{	
			return;
		}
		else
		{
			subcategory[last_subcat].className += " selected_item";
			subcaption = " - " + subcategory[last_subcat].text;
			show_subcaption();
			return;
		}
	}
	else if (id == -2)
	{
		if (last_subcat != -1)
		{
			subcategory[last_subcat].className = subcategory[last_subcat].className.replace("selected_item", "");
			subcaption = "";
			show_subcaption();
			last_subcat = -1;
		}
		return;
	}

	if (last_subcat != -1)
	{
		subcategory[last_subcat].className = subcategory[last_subcat].className.replace(" selected_item", "");
	}
	subcategory[id].className += " selected_item";
	subcaption = " - " + subcategory[id].text;
	show_subcaption();
	
	last_subcat = id;
	set_cookies();
}

// function for showing panel(logPanel or cartPanel)
function showPanel(id) {
    // hide panel for logging on
    hidePanel();

    var p = document.getElementById(String(id));
    if(p == null)
        return;

    if (p.style.display === "none") {
        p.style.display = "block";
    } 
    else {
        p.style.display = "none";
    }
}

// function for hiding panels for logging on and cart panel
function hidePanel() {
    var lp = document.getElementById('logPanel');
    lp.style.display = 'none';
		
	var up = document.getElementById('userPanel');
    up.style.display = 'none';

    var cp = document.getElementById('cartPanel');
    cp.style.display = 'none';
}



$(document).ready(function() {
	// hide panel if it is displayed
	$(document).click(function(){
		if($('#logPanel').display != 'none') {
			hidePanel();
		}
		else if($('#userPanel').display != 'none') {
			hidePanel();
		}
		// hide terms and conditions when is clicked outer 
		if($('#termsAndConditions').display != 'none') {
			$('#termsAndConditions').css('display', 'none');
		}
	});
	
	// clicked on "Přihlásit se"
	$('#logRef').click(function(event){
		event.stopPropagation();  // stop propagation to enable display logPanel
		showPanel("logPanel");
	});
	
	$('#userRef').click(function(event){
		event.stopPropagation();  // stop propagation to enable display userPanel
		showPanel("userPanel");
	});

	// clicked on logPanel -> stopPrapagation
	$('#logPanel').click(function(event){
		event.stopPropagation();  // stop propagation to disable hide logPanel
	});
	
	// clicked on userPanel -> stopPrapagation
	$('#userPanel').click(function(event){
		event.stopPropagation();  // stop propagation to disable hide userPanel
	});

	// mouse enter on "Košík"
	$('#cartRef').mouseenter(function(){
		hidePanel();
		showPanel("cartPanel");
	});

	// mouse leave cart panel
	$('#cartPanel').mouseleave(function(){
		hidePanel();
	});

	// clicked on cartPanel -> stopPrapagation
	$('#cartPanel').click(function(event){
		event.stopPropagation();
	});

	// clicked on button for hiding terms
	$('#hideTermsButton').click(function() {
		$('#termsAndConditions').css('display', 'none');
	});
	
	// clicked on X for hiding terms
	$('#hideTermsX').click(function() {
		$('#termsAndConditions').css('display', 'none');
	});

	// clicked on termsAndConditions -> stopPrapagation
	$('#termsAndConditions').click(function(event){
		event.stopPropagation();
	});

	// continue to next step
  	$('#next1').click(function() {
		$('#cartStep1').css('display', 'none');
		$('#cartStep2').css('display', 'block');
	});
	
	// continue to last step
	$('#next2').click(function() {
		// check if radiopay was selected
		var selected = $("input[type='radio'][name='radiopay']:checked");
		if (selected.length <= 0) {
			$('#badRadioPay').css('display', 'block');
			return;
		}

		// check license agreement
		var terms = document.getElementById('termsID');
		if(!terms.checked) {
			$('#badTerms').css('display', 'block');
			return;
		}

		$('#cartStep2').css('display', 'none');
		// hide possible errors
		$('#badTerms').css('display', 'none');
		$('#badRadioPay').css('display', 'none');
		// show last step
		$('#cartStepFinish').css('display', 'block');
	});

	// last step confirmed
	$('#finish').click(function() {
		alert('Rezervováno!');
	});
	
	// return to first step
	$('#back1').click(function() {
		$('#cartStep2').css('display', 'none');
		$('#cartStep1').css('display', 'block');
	});

	// return to second step
	$('#back2').click(function() {
		$('#cartStepFinish').css('display', 'none');
		$('#cartStep2').css('display', 'block');
	});

	// hide error div when terms was not checked
	$('#closeBadTerms').click(function() {
		$('#badTerms').css('display', 'none');
	});

	// hide error div when payment was not selected
	$('#closeBadRadioPay').click(function() {
		$('#badRadioPay').css('display', 'none');
	});

	// terms of service agreement checkbox
	$('#termsText').click(function(){
		var terms = document.getElementById('termsID');
		if(terms.checked)
			terms.checked = false;
		else
			terms.checked = true;
	});

	// set paragraph payFinal value in final step
	$('input[type=radio][name=radiopay]').change(function() {
		var selectedVal = "";
		var selected = $("input[type='radio'][name='radiopay']:checked");
		if (selected.length > 0) {
			selectedVal = selected.val();
		}
		
		$('#payFinal').text(selectedVal);
	});

	// set paragraph for notes in final step
	$('#notesID').on('input', function() {
		var content = $(this).val();
		$('#notes').text(content);
	});

	// clicked on show terms option in footer
	$('#showTerms').click(function(event){
		$('#termsAndConditions').css('display', 'block');
		event.stopPropagation();
	});

	// clicked on show terms option in footer
	$('#showTermsReg').click(function(event){
		$('#termsAndConditions').css('display', 'block');
		event.stopPropagation();
	});

	// clicked on show terms option in footer
	$('#showTermsCart').click(function(){
		$('#termsAndConditions').css('display', 'block');
		event.stopPropagation(event);
	});

	// /* DATEPICKER */
	// // set custom language CZ
	// $.datepicker.regional['cs'] = { 
	// 	closeText: 'Cerrar', 
	// 	prevText: 'Předchozí', 
	// 	nextText: 'Další', 
	// 	currentText: 'Hoy', 
	// 	monthNames: ['Leden','Únor','Březen','Duben','Květen','Červen', 'Červenec','Srpen','Září','Říjen','Listopad','Prosinec'],
	// 	monthNamesShort: ['Led','Ún','Bře','Dub','Kvě','Čen', 'Čec','Srp','Zář','Říj','Lis','Pro'], 
	// 	dayNames: ['Neděle','Pondělí','Úterý','Středa','Čtvrtek','Pátek','Sobota'], 
	// 	dayNamesShort: ['Ne','Po','Út','St','Čt','Pá','So',], 
	// 	dayNamesMin: ['Ne','Po','Út','St','Čt','Pá','So'], 
	// 	weekHeader: 'Sm', 
	// 	dateFormat: 'dd.mm.yy', 
	// 	firstDay: 1, 
	// 	isRTL: false, 
	// 	showMonthAfterYear: false, 
	// 	yearSuffix: ''}; 

	// $.datepicker.setDefaults($.datepicker.regional['cs']);


	// //https://stackoverflow.com/questions/15400775/jquery-ui-datepicker-disable-array-of-dates
	// var startDate = "2017-11-15", // some start date
    // endDate  = "2017-11-21",  // some end date
    // dateRange = [];           // array to hold the range

	// // populate the array
	// for (var d = new Date(startDate); d <= new Date(endDate); d.setDate(d.getDate() + 1)) {
	// 	dateRange.push($.datepicker.formatDate('yy-mm-dd', d));
	// }

	// // format to sql with this way -> $( "#datepickerFrom" ).datepicker({dateFormat: "mm-dd-yy"});
	// $('#datepickerFromIcon').click(function(){
	// 	// TODO
	// 	$('#datepickerFrom').datepicker({
	// 		changeMonth: true, 
	// 		changeYear: true,
	// 		showAnim: "slideDown"
	// 	});
	// });
	// $('#datepickerFrom').datepicker({
	// 	changeMonth: true, 
	// 	changeYear: true,
	// 	showAnim: "slideDown",
	// 	beforeShowDay: function(date){
	// 		var string = jQuery.datepicker.formatDate('yy-mm-dd', date);
	// 		return [ dateRange.indexOf(string) == -1 ]
	// 	}
	// });

	// $('#datepickerToIcon').click(function(){
	// 	// TODO
	// 	$('#datepickerTo').datepicker({
	// 		changeMonth: true, 
	// 		changeYear: true,
	// 		showAnim: "slideDown"
	// 	});
	// });
	// $('#datepickerTo').datepicker({
	// 	changeMonth: true, 
	// 	changeYear: true,
	// 	showAnim: "slideDown"
	// });
});
