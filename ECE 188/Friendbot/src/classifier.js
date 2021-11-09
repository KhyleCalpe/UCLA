//STATES and VARIABLES for ENTIRE SYSTEM
var emotion = ""; //<- just set this manually for now until we get input from neural network
var select = 0;
var endpage_select = "none";
var state = "welcome";
var text_input = "";
var old_text_input = "";
var result = "";
var recommendation_text = "no suggestion yet";

var today;
var dd;
var mm;
var yyyy;
var time;
var date_stuff = new Date();

const URL = "https://teachablemachine.withgoogle.com/models/F1ErHadTR/";
let model, webcamInit, webcam, maxPredictions, predictCounter, currGuess, pastGuess;

window.onload = function clearStuff(){
	document.getElementById("userInput").style.display = "none";
	document.getElementById("submit").style.display = "none";
	document.getElementById("record").style.display = "none";
}

// Load the image model and setup the webcam
async function init() {
	const modelURL = URL + 'model.json';
	const metadataURL = URL + 'metadata.json';

	// load the model and metadata
	// Refer to tmImage.loadFromFiles() in the API to support files from a file picker
	// or files from your local hard drive
	model = await tmImage.load(modelURL, metadataURL);
	maxPredictions = model.getTotalClasses();

	// Convenience function to setup a webcam
	const flip = true; // whether to flip the webcam
	webcam = new tmImage.Webcam(500, 500, flip); // width, height, flip
	await webcam.setup(); // request access to the webcam
	webcam.play();
	window.requestAnimationFrame(loop);

	// append elements to the DOM
	document.getElementById('webcam').appendChild(webcam.canvas);

	webcamInit = true;
}

async function loop() {
	webcam.update(); // update the webcam frame
	await predict();
	await writeGuess();
	window.requestAnimationFrame(loop);
}

// run the webcam image through the image model
async function predict() {
	// predict can take in an image, video or canvas html element
	const prediction = await model.predict(webcam.canvas);
	// base reference
	guessProb = 0;
	baseGuess = -1;
	// check other predictions 
	for (let i = 0; i < maxPredictions; i++) {
		currProb = prediction[i].probability;
		if (currProb > guessProb) {
			baseGuess = i;
			guessProb = currProb; 
		}
	}
	// set currGuess
	if (currGuess == baseGuess) {
		predictCounter++;
	} else {
		currGuess = baseGuess;
		predictCounter = 0;
	}
}

async function writeGuess() {
	if (predictCounter < 30) return;
	switch(currGuess) {
		case 0:
			emotion = "angry";
			break;
		case 1:
			emotion = "happy";
			break;
		case 2:
			emotion = "sad";
			break;
		case 3:	
			emotion = "neutral";
			break;
		case 4:
			emotion = "fear";
			break; 
	}
	document.getElementById("loading_text").innerHTML = emotion;
	predictCounter = 0;
	return;
}

function stateControl(){
	if(webcamInit){
		document.getElementById('webcam').removeChild(webcam.canvas);
		webcam.stop();
		webcamInit = false;
	}
	if(state == "welcome"){
		showLoad();
		if(emotion == "neutral"){
			state = "question_setup_neutral";
		}
		else{
			state = "question_setup_normal";
		}
	}
	else if(state == "question_setup_normal"){
		confirmEmotion();
		state = "confirm_wait_state";
	}
	else if(state == "question_setup_neutral"){
		displayEmotionOptions();
		state = "determine_wait_state";
	}
	else if(state == "confirm_wait_state"){
		if(select == 1){
			if(emotion == "angry"){
				agreement();
				state = "anger_one";
			}
			else if(emotion == "fear"){
				agreement();
				state = "fear_one";
			}
			else if(emotion == "happy"){
				agreement();
				state = "happy_one";
			}
			else if(emotion == "sad"){
				agreement();
				state = "sad_one";
			}
		}
		else{
			displayEmotionOptions();
			state = "determine_wait_state";
		}
	}
	else if(state == "determine_wait_state"){
		if(emotion == "angry"){
			agreement();
			state = "anger_one";
		}
		else if(emotion == "fear"){
			agreement();
			state = "fear_one";
		}
		else if(emotion == "happy"){
			agreement();
			state = "happy_one";
		}
		else if(emotion == "sad"){
			agreement();
			state = "sad_one";
		}
	}
//CONTROL FOR QUESTIONNAIRE QUESTIONS - - - - - - - - - -

//HAPPY - - - - - - - - - -- - - - - 
	else if(state == "happy_one"){
		happyOne();
		state = "happyOne_wait_state";
	}
	else if(state == "happyOne_wait_state"){
		if(select == 1){
			happy21();
			state = "happy_21";
		}
		if(select == 0){
			happy22();
			state = "happy_22";
		}
	}
	else if(state == "happy_21"){
		if(select == 1){
			happy211();
			state = "endpage";
		}
		if(select == 0){
			happy212();
			state = "endpage";
		}
			
	}
	else if(state == "happy_22"){
		if(select == 1){
			happy221();
			state = "endpage";
		}
		if(select == 0){
			happy222();
			state = "endpage";
		}
	}

//ANGRY - - - - - - - - - - - - - - -
	else if(state == "anger_one"){
		angerOne();
		state = "angerOne_wait_state";
	}
	else if(state == "angerOne_wait_state"){
		if(select == 1){
			anger21();
			state = "anger21";
		}
		if(select == 0){
			anger22();
			state = "anger22";
		}
	}
	else if(state == "anger21"){
		if(select == 1){
			anger211();
			state = "anger211";
		}
		if(select == 0){
			anger212();
			state = "anger212";
		}
	}
	else if(state == "anger22"){
		if(select == 1){
			anger221();
			state = "anger221";
		}
		if(select == 0){
			anger222();
			state = "anger222";
		}
	}
	else if(state == "anger211"){
		if(select == 1){
			anger2111();
			state = "anger2111";
		}
		if(select == 0){
			anger2112();
			state = "endpage";
		}
	}
	else if(state == "anger212"){
		if(select == 1){
			anger2121();
			state = "anger2121";
		}
		if(select == 0){
			anger2122();
			state = "endpage";
		}
	}
	else if(state == "anger221"){
		if(select == 1){
			anger2211();
			state = "endpage";
		}
		if(select == 0){
			anger2212();
			state = "endpage";
		}
	}
	else if(state == "anger222"){
		if(select == 1){
			anger2221();
			state = "anger2221";
		}
		if(select == 0){
			anger2222();
			state = "anger2222";
		}
	}


	else if(state == "anger2111"){
		if(select == 1){
			anger21111();
			state = "endpage";
		}
		if(select == 0){
			anger21112();
			state = "211111";
		}
	}
	else if(state == "anger2121"){
		if(select == 1){
			anger21211();
			state = "endpage";
		}
		if(select == 0){
			anger21212();
			state = "endpage";
		}
	}
	else if(state == "anger2221"){
		if(select == 1){
			anger22211();
			state = "endpage";
		}
		if(select == 0){
			anger22212();
			state = "endpage";
		}
	}
	else if(state == "anger2222"){
		if(select == 1){
			anger22221();
			state = "endpage";
		}
		if(select == 0){
			anger22222();
			state = "endpage";
		}
	}
	else if(state == "anger211111"){
		if(select == 1){
			anger2111111();
			state = "endpage";
		}
		if(select == 0){
			anger2111112();
			state = "endpage";
		}
	}

//FEAR - - - - - - - - - - - - - - - - - - 
	else if(state == "fear_one"){
		fearOne();
		state = "fearOne_wait_state";
	}
	else if(state == "fearOne_wait_state"){
		if(select == 1){
			fear21();
			state = "fear21";
		}
		if(select == 0){
			fear22();
			state = "fear22";
		}
	}
	else if(state == "fear21"){
		if(select==1){
			fear211();
			state = "endpage";
		}
		if(select==0){
			fear212();
			state = "fear212";
		}
	}
	else if(state == "fear22"){
		if(select==1){
			fear221();
			state = "fear221";
		}
		if(select==0){
			fear222();
			state = "endpage"
		}
	}
	else if(state == "fear212"){
		if(select==1){
			fear2121();
			state = "endpage";
		}
		if(select==0){
			fear2122();
			state = "2122"
		}
	}
	else if(state == "fear221"){
		if(select==1){
			fear2211();
			state = "endpage";
		}
		if(select==0){
			fear2212();
			state = "endpage"
		}
	}
	else if(state == "2122"){
		if(select==1){
			fear21221();
			state = "21221";
		}
		if(select==0){
			fear21222();
			state = "21222"
		}
	}
	else if(state == "21221"){
		if(select==1){
			fear212211();
			state = "endpage";
		}
		if(select==0){
			fear212212();
			state = "endpage"
		}
	}
	else if(state == "21222"){
		if(select==1){
			fear212221();
			state = "endpage";
		}
		if(select==0){
			fear212222();
			state = "endpage"
		}
	}

//SAD - - - - - - - - - - - - - - - - - - 
	else if(state == "sad_one"){
		sadOne();
		state = "sadOne_wait_state";
	}
	else if(state == "sadOne_wait_state"){
		if(select == 1){
			sad21();
			state = "sad21";
		}
		if(select == 0){
			sad22();
			state = "sad22";
		}
	}
	else if(state =="sad21"){
		if(select == 1){
			sad211();
			state = "sad211";
		}
		if(select == 0){
			sad212();
			state = "sad212";
		}
	}
	else if(state == "sad22"){
		if(select == 1){
			sad221();
			state = "sad221";
		}
		if(select == 0){
			sad222();
			state = "sad222";
		}
	}
	else if(state == "sad211"){
		if(select == 1){
			sad2111();
			state = "sad2111";
		}
		if(select == 0){
			sad2112();
			state = "endpage";
		}
	}
	else if(state == "sad212"){
		if(select == 1){
			sad2121();
			state = "sad2121";
		}
		if(select == 0){		//<-special jump case
			sad222();
			state = "sad222";
		}
	}
	else if(state == "sad221"){
		if(select == 1){
			sad2211();
			state = "endpage";
		}
		if(select == 0){
			sad2212();
			state = "endpage";
		}
	}
	else if(state == "sad222"){
		if(select == 1){
			sad2221();
			state = "sad2221";
		}
		if(select == 0){
			sad2222();
			state = "sad2222";
		}
	}
	else if(state == "sad2111"){
		if(select == 1){
			sad21111();
			state = "endpage";
		}
		if(select == 0){
			sad21112();
			state = "sad21112";
		}
	}
	else if(state == "sad2121"){
		if(select == 1){
			sad21211();
			state = "endpage";
		}
		if(select == 0){
			sad21212();
			state = "endpage";
		}
	}
	else if(state == "sad2221"){
		if(select == 1){
			sad22211();
			state = "endpage";
		}
		if(select == 0){
			sad22212();
			state = "endpage";
		}
	}
	else if(state == "sad2222"){
		if(select == 1){
			sad22221();
			state = "endpage";
		}
		if(select == 0){
			sad22222();
			state = "endpage";
		}
	}
	else if(state == "sad21112"){
		if(select == 1){
			sad211121();
			state = "endpage";
		}
		if(select == 0){
			sad211122();
			state = "endpage";
		}
	}



//CONTROL FOR ENDPAGE AND MORE  - - - - - - 
	else if(state == "endpage"){
		endpage();
		state = "endpage_wait_state";
	}
	else if(state == "endpage_wait_state"){
		if(endpage_select == "suggestion"){
			suggestion();
			state = "endpage";

		}
		else if(endpage_select == "record"){
			record();
			state = "endpage";
		}
		else if(endpage_select == "restart"){
			startPage();
			state = "welcome";
		}
	}



}

//END PAGE
function endpage(){
	document.getElementById("skip_button").style.display = "none";

	document.getElementById("record").style.display = "none";
	document.getElementById("record_button").style.display = "block";

	document.getElementById("restart_button").style.display = "block";

	document.getElementById("recommmendation").style.display = "none";
	document.getElementById("suggestion_button").style.display = "block";

	document.getElementById("continue_q_button").style.display = "none";
	document.getElementById("questionnaire_home").style.display = "none";

	if(old_text_input == text_input){
		text_input = "";
	}

}

function suggestion(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("recommmendation").style.display = "block";
	document.getElementById("record_button").style.display = "none";
	document.getElementById("restart_button").style.display = "none";
	document.getElementById("suggestion_button").style.display = "none";
	document.getElementById("recommmendation").innerHTML = recommendation_text;
	document.getElementById("record_button").style.display = "none";
	document.getElementById("restart_button").style.display = "none";


}

function record(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("record").style.display = "block";
	document.getElementById("record_button").style.display = "none";
	document.getElementById("restart_button").style.display = "none";
	document.getElementById("record").innerHTML = result;

	document.getElementById("suggestion_button").style.display = "none";
	document.getElementById("recommmendation").style.display = "none";


}

function updateResult(){

	today = new Date();
	dd = today.getDate();

	mm = today.getMonth()+1; 
	yyyy = today.getFullYear();

	if(dd<10) 
	{
	    dd='0'+dd;
	} 

	if(mm<10) 
	{
	    mm='0'+mm;
	} 
	
	today = dd+'/'+mm+'/'+yyyy;
	time = date_stuff.getHours() + ":" + date_stuff.getMinutes();

	result = result.concat("Date: " +today+ " Time: "+ time + " Feeling: " +emotion+ " Note: \n");

}

//QUESTIONNAIRE

//for SAD emotion - - - - - - - - - - 
function sadOne(){
	document.getElementById("skip_button").style.display = "none";

	document.getElementById("continue_q_button").style.display = "none";
	document.getElementById("yes_button").style.display = "block";
   	document.getElementById("no_button").style.display = "block";
	document.getElementById("questionnaire_home").innerHTML = "Did something happen that made you sad?";
}
function sad21(){
	document.getElementById("questionnaire_home").innerHTML = "Was another person involved?";
}
function sad22(){
	document.getElementById("questionnaire_home").innerHTML = "Are you sad because of what happened to someone else?";
}
function sad211(){
	document.getElementById("questionnaire_home").innerHTML = "Was anyone physically hurt?";
}
function sad212(){
	document.getElementById("questionnaire_home").innerHTML = "Are you angry with yourself?";
}
function sad221(){
	document.getElementById("questionnaire_home").innerHTML = "Were they physically injured?";
}
function sad222(){
	document.getElementById("questionnaire_home").innerHTML = "Do you know why you're sad?";
}
function sad2111(){
	document.getElementById("questionnaire_home").innerHTML = "Are you the one injured?";
}
function sad2112(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "That sucks, but least no one was hurt.";
	recommendation_text = "I recommend talking to the person involved if you think it could resolve your issues.";

}
function sad2121(){
	document.getElementById("questionnaire_home").innerHTML = "Did you hurt yourself?";
}

function sad2211(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "They should seek some medical attention.";
	recommendation_text = "";

}
function sad2212(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "That sucks, but at least no one got hurt.";
	recommendation_text = "";

}
function sad2221(){
	document.getElementById("questionnaire_home").innerHTML = "Are you able to cope with your sadness?";
}
function sad2222(){
	document.getElementById("questionnaire_home").innerHTML = "Have you felt this way for more than a week?";
}
function sad21111(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You should seek some medical attention.";
	recommendation_text = "I recommend talking to the person involved if you think it could resolve your issues.";

}
function sad21112(){
	document.getElementById("questionnaire_home").innerHTML = "Did you hurt them?";
}
function sad21211(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You should seek some medical attention.";
	recommendation_text = "I recommend talking to the person involved if you think it could resolve your issues.";

}
function sad21212(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "That sucks, but least you're not hurt";
	recommendation_text = "I recommend talking to the person involved if you think it could resolve your issues.";

}
function sad22211(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "That's good, at least it doesn't affect your daily life.";
	recommendation_text = "I recommend talking to a friend about your issues. Maybe they can offer some support and suggestions.";

}
function sad22212(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You should seek some professional help.";
	recommendation_text = "...";

}
function sad22221(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You should seek some professional help.";
	recommendation_text = "...";

}
function sad22222(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Give it some time, maybe you'll feel better tomorrow.";
	recommendation_text = "I recommend talking to a friend about your issues. Maybe they can offer some support and suggestions.";

}
function sad211121(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Oh man";
	recommendation_text = "...";

}
function sad211122(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Hopefully they get better, but at least you're not hurt.";
	recommendation_text = "I recommend talking to the person involved if you think it could resolve your issues.";

}

//for FEAR emotion - - - - - - - - - -
function fearOne(){
	document.getElementById("skip_button").style.display = "none";

	document.getElementById("continue_q_button").style.display = "none";
	document.getElementById("yes_button").style.display = "block";
   	document.getElementById("no_button").style.display = "block";
	document.getElementById("questionnaire_home").innerHTML = "Did something happen that made you scared?";
}
function fear21(){
	document.getElementById("questionnaire_home").innerHTML = "Is something threatening your life?";
}
function fear22(){
	document.getElementById("questionnaire_home").innerHTML = "Do you know what you're afraid of?";
}
function fear211(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Oh no!";
}
function fear212(){
	document.getElementById("questionnaire_home").innerHTML = "Does your fear affect your daily life?";
}
function fear221(){
	document.getElementById("questionnaire_home").innerHTML = "Is something threatening your life?";
}
function fear222(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You may have anxiety, you should seek some professional help.";
}
function fear2121(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You may have anxiety, you should seek some professional help.";
}
function fear2122(){
	document.getElementById("questionnaire_home").innerHTML = "Was this fear caused by a single incident?";
}
function fear2211(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Oh no!";
	recommendation_text = "Call for help!";

}
function fear2212(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You may have anxiety, you should seek some professional help.";
	recommendation_text = "...";
}
function fear21221(){
	document.getElementById("questionnaire_home").innerHTML = "Was it because you went to a haunted house?";
}
function fear21222(){
	document.getElementById("questionnaire_home").innerHTML = "Has this been going on for more than a week?";
}
function fear212211(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Aw man, I'm very scared of haunted houses too.";
	recommendation_text = "I recommend not giving into peer pressure and going to haunted houses if you don't like them.";
}
function fear212212(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Aww, well hopefully what scared you doesn't happen again.";
	recommendation_text = "I recommend avoiding things that scare you if they don't help you grow as a person.";
}
function fear212221(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You should seek some help.";
	recommendation_text = "I recommend talking to a professional about your recurring fears.";
}
function fear212222(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Hopefully things get better soon.";
	recommendation_text = "I recommend talking to a friend about your fears.";
}

//for ANGRY emotion
function angerOne(){
	document.getElementById("skip_button").style.display = "none";

	document.getElementById("continue_q_button").style.display = "none";
	document.getElementById("yes_button").style.display = "block";
   	document.getElementById("no_button").style.display = "block";
	document.getElementById("questionnaire_home").innerHTML = "Did something happen that made you mad?";
}

function anger21(){
	document.getElementById("questionnaire_home").innerHTML = "Was another person involved?";
}

function anger22(){
	document.getElementById("questionnaire_home").innerHTML = "Are you mad because of what happened to someone else?";
}

function anger211(){
	document.getElementById("questionnaire_home").innerHTML = "Was anyone physically hurt?";
}

function anger212(){
	document.getElementById("questionnaire_home").innerHTML = "Are you angry with yourself?";
}

function anger221(){
	document.getElementById("questionnaire_home").innerHTML = "Were they physically injured?";
}

function anger222(){
	document.getElementById("questionnaire_home").innerHTML = "Do you know why you're angry?";
}

function anger2111(){
	document.getElementById("questionnaire_home").innerHTML = "Are you the one injured?";
}
function anger2112(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "That sucks, but least no one was hurt.";
	recommendation_text = "I recommend talking to the person you are angry at because it may resolve your issues.";
}

function anger2121(){
	document.getElementById("questionnaire_home").innerHTML = "Did you hurt yourself?";
}
function anger2122(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Why?";
}
function anger2211(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "They should seek some medical attention";
	recommendation_text = "";
}
function anger2212(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "That sucks, but at least no one got hurt.";
	recommendation_text = "";
}
function anger2221(){
	document.getElementById("questionnaire_home").innerHTML = "Are you able to cope your anger?";
}
function anger2222(){
	document.getElementById("questionnaire_home").innerHTML = "Have you felt this way for more than a week?";
}
function anger21111(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You should seek some medical attention.";
	recommendation_text = "I recommend talking to the person you are angry at because it may resolve your issues.";
}
function anger21112(){
	document.getElementById("questionnaire_home").innerHTML = "Did you hurt them?";
}
function anger21211(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You should seek some medical attention.";
	recommendation_text = "I recommend talking to the person you are angry at because it may resolve your issues.";
}
function anger21212(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "That sucks, but at least no one got hurt.";
	recommendation_text = "I recommend talking to the person you are angry at because it may resolve your issues.";
}
function anger22211(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "That's good, at least it doesn't affect your daily life.";
	recommendation_text = "I recommend talking to a friend about your issues. Maybe they can offer some support and suggestions.";
}
function anger22212(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You should seek some professional help.";
	recommendation_text = "...";
}
function anger22221(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "You should seek some professional help.";
	recommendation_text = "...";
}
function anger22222(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Give it some time, maybe you'll feel better tomorrow.";
	recommendation_text = "I recommend talking to a friend about your issues. Maybe they can offer some support and suggestions.";
}
function anger2111111(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Oh man.";
	recommendation_text = "...";
}
function anger2111112(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "Hopefully they get better, but at least you're not hurt.";
	recommendation_text = "I recommend talking to the person you are angry at because it may resolve your issues.";

}


//for HAPPY emotion
function happyOne(){
	document.getElementById("skip_button").style.display = "none";

	document.getElementById("continue_q_button").style.display = "none";
	document.getElementById("yes_button").style.display = "block";
   	document.getElementById("no_button").style.display = "block";
	document.getElementById("questionnaire_home").innerHTML = "Is today a special day?";
}

function happy21(){
	document.getElementById("questionnaire_home").innerHTML = "Is it your birthday?";
}

function happy22(){
	document.getElementById("questionnaire_home").innerHTML = "Did you accomplish something great recently?";
}

function happy211(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";

	document.getElementById("questionnaire_home").innerHTML = "Happy birthday!";
	recommendation_text = "";
}

function happy212(){


	document.getElementById("userInput").style.display = "block";
	document.getElementById("submit").style.display = "block";

	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "What's the occasion?";
	recommendation_text = "";

}

function happy221(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
	document.getElementById("questionnaire_home").innerHTML = "What was it?";
	recommendation_text = "";
}

function happy222(){
	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";

	document.getElementById("questionnaire_home").innerHTML = "Nice, keep it up!";
	recommendation_text = "";

}

//FIRST PAGE and LOAD PAGE
function startPage(){
	document.getElementById("suggestion_button").style.display = "none";
	document.getElementById("record_button").style.display = "none";
	document.getElementById("restart_button").style.display = "none";

	document.getElementById("welcomeDiv").style.display = "block";
	document.getElementById("begin_button").style.display = "block";

}

function showLoad() {
   document.getElementById("loading_text").style.display = "block";
   document.getElementById("continue_button").style.display = "block";
   document.getElementById("welcomeDiv").style.display = "none";
   document.getElementById("begin_button").style.display = "none";
   document.getElementById("webcam").style.display = "block";
   init();
}

//QUESTIONNAIRE SETUP

function confirmEmotion(){
	document.getElementById("loading_text").style.display = "none";
    document.getElementById("continue_button").style.display = "none";
    document.getElementById("questionnaire_home").style.display = "block";
	document.getElementById("yes_button").style.display = "block";
   	document.getElementById("no_button").style.display = "block";

    document.getElementById("questionnaire_home").innerHTML = "You seem to be " + emotion + ", is this accurate?";

}

function displayEmotionOptions(){
	document.getElementById("loading_text").style.display = "none";
	document.getElementById("continue_button").style.display = "none";
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";

	document.getElementById("questionnaire_home").style.display = "block";
	document.getElementById("angry_button").style.display = "block";
	document.getElementById("fear_button").style.display = "block";
	document.getElementById("happy_button").style.display = "block";
	document.getElementById("sad_button").style.display = "block";

	document.getElementById("questionnaire_home").innerHTML = "I can't seem to tell how you're feeling, which do you think best describes how you feel now?";
}
  
function agreement(){
	document.getElementById("yes_button").style.display = "none";
   	document.getElementById("no_button").style.display = "none";
   	document.getElementById("angry_button").style.display = "none";
	document.getElementById("fear_button").style.display = "none";
	document.getElementById("happy_button").style.display = "none";
	document.getElementById("sad_button").style.display = "none";

	document.getElementById("skip_button").style.display = "block";


   	document.getElementById("continue_q_button").style.display = "block";
	document.getElementById("questionnaire_home").innerHTML = "Great, we're on the same page.";

	updateResult();
}

function continue_two()
{

	text_input = document.getElementById("userInput").value;
	//alert(text_input);
	document.getElementById("userInput").style.display = "none";
	document.getElementById("submit").style.display = "none";

	//console.log(text_input);
	stateControl();
	result = result.substring(0, result.length-2);
	result = result + text_input + "\n";
	old_text_input = text_input;

}

function skipControl()
{
	state = "endpage";
	stateControl();
	text_input = "";
}

// INPUT SETUP
function yesClicked(){
	select = 1;
	stateControl();
}
function noClicked(){
	select = 0;
	stateControl();
}
function angryClicked(){
	emotion = "angry";
	stateControl();
}
function afraidClicked(){
	emotion = "fear";
	stateControl();
}
function happyClicked(){
	emotion = "happy";
	stateControl();	
}
function sadClicked(){
	emotion = "sad";
	stateControl();	
}
function suggestionClicked(){
	endpage_select = "suggestion";
	stateControl();	
}
function recordClicked(){
	endpage_select = "record";
	stateControl();	
}
function restartClicked(){
	endpage_select = "restart";
	stateControl();
}