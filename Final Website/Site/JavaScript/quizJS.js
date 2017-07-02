/* Code to make the quiz work.
   Concept from YouTube video: https://www.youtube.com/watch?v=d_UuOVhuCF8 */

var choiceA, choiceB, choiceC, choiceD, correctAnswers = 0, currentQuestion = 0;

/* Store the questions, their answer options and the location of the correct answer */
var questions = [ 
	[ "Define HTTP?", "HyperText Transfer Protocol", "Have Text Transferred Promptly", "HugeText Transfer Protocol", "HyperText Technology Protocol", "A" ],
	[ "What Are DNS Servers Used For?", "Accessing the Internet", "Retrieving an IP address' URL", "Retrieving a URL's IP Address", "Loading a Web Page", "C" ],
	[ "In Terms of the Internet, What Does a Packet Consist Of?", "Header, Body & Footer", "DNS Address & Content", "IP Address & Document", "Header & Body", "D" ],
	
	[ "In Which of These Tags Would You Write Your Site's HTML Code?", "&lt p &gt", "&lt body &gt", "&lt html &gt", "&lt table &gt", "C" ],
	[ "Which One of the Following Image Implementations Is Valid HTML Code?", "&lt img src = 'myImage' /&gt", "&lt img src = 'myImage.jpg' &gt", "&lt img src = 'myImage.jpg' /&gt", "&lt img src = 'myImage.jpg' height = 100 /&gt", "C" ],
	[ "Which of These Tags Would You Use to Add a URL Link?", "&lt a href &gt", "&lt URLLink &gt", "&lt IPLink &gt", "&lt Linksrc &gt", "A" ],
	
	[ "Which of the Following Attributes Would You Use to Create White Space between the given Element & Others?", "Border", "Margin", "Padding", "Width", "B" ],
	[ "Which of the Following CSS Positioning Attributes Would Position the Element Relative to the Browser Window?", "Fixed", "Relative", "Float", "Absolute", "A" ],
	[ "Define CSS?", "Computer's Simple Standardisation", "Cascading Style Sheets", "Complex Style Sheets", "Conjuring Simple Style", "B"],
	[ "What Would Be the Best Approach for Positioning an Image with Some Text to the Right Hand Side of a Page?", "Apply CSS and Float Both Individually", "Place Both Elements within a Div Tag and Float the Div to the Right", "Use Fixed Positioning on Both Elements", "Use Relative Positioning on Both Elements", "B" ] ];

/* Display the results once the quiz has finished */
results = function(){
	var section = document.getElementById( "quiz" );
	section.style.minHeight = "0px";
	section.innerHTML = "<p align = 'center' height = 60px> You Got <b> <font size = '5'>" + correctAnswers + "/" + questions.length + "</font> </b> Answers Correct.</p>";
}	
	
/* Move on to the next question, ends if all questions completed */
nextQuestion = function(){
	
	if( currentQuestion == questions.length ){ // if all questions done.
		results();
	}
	
	renderQuestion( currentQuestion, questions[currentQuestion][0], questions[currentQuestion][1], questions[currentQuestion][2], questions[currentQuestion][3], questions[currentQuestion][4] );
	
}

/* Check if the answer selected was correct */
checkAnswer = function(){
	
	/* Find which answer was given */
	choices = document.getElementsByName("choices");
	for( var i = 0; i < choices.length; i++){
		if(choices[i].checked){
			var response = choices[i].value;
		}
	}
	
	/* If correct, add to the correct answer score */
	if( questions[currentQuestion][5] == response ){
		correctAnswers ++;
	}
	
	/* Move on to the next question */
	currentQuestion ++;
	
	nextQuestion(); 
}

/* Render the question, including: the question, its answer options, and the submit answer button */
renderQuestion = function( questionNumber, question, choiceA, choiceB, choiceC, choiceD ){
	
	var section = document.getElementById( "quiz" );
	
	section.innerHTML = "<h2 align = 'left'> Question " + ( questionNumber + 1 ) + "/" + questions.length + "</h2>";
	section.innerHTML += "<h2 align = 'center'>" + question + "</h2>";
	section.innerHTML += "<table align = 'center' width = 75% cellPadding = '5'> <tr> <td align = 'center'> <input type = 'radio' name = 'choices' value = 'A'>" + choiceA + "</td> <td align = 'center'> <input type = 'radio' name = 'choices' value = 'B'> " + choiceB + "</td> </tr> <td align = 'center'> <input type = 'radio' name = 'choices' value = 'C'> " + choiceC + "</td> <td align = 'center'> <input type = 'radio' name = 'choices' value = 'D'>" + choiceD + "</td> </tr> </table> <br>";

	section.innerHTML += "<div align = 'center'> <button onclick='checkAnswer()' align = 'center'>Submit Answer</button> </div> <br>";
	
}

/* Load the quiz as soon as the page loads */
window.onload = function(){
	
	var section = document.getElementById( "quiz" );
	section.style.minHeight = "150px";
	section.style.margin = "10px";
	
	nextQuestion();
	
}
