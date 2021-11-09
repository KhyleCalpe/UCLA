$(function(){	
	var handle = $( "#numbers" );
    var $write = $('#write'),
		shift = false,
		capslock = false;
        punctuation = false;

    $('.ui-slider-handle').height(16);
    $('.ui-slider-handle').width(18);  
    $( "#slider" ).slider({
      min: 0,
      max: 9,
      create: function() {
        handle.text( $( this ).slider( "value" ) );
      },
      slide: function( event, ui ) {
        handle.text( ui.value );
      },
      stop: function( event, ui ) {
        $write.html($write.html() + ui.value);
      }
    });

	$('#keyboard li').click(function(){
		var $this = $(this),
			character = $this.html(); // If it's a lowercase letter, nothing happens to this variable
		
		// Emoji key
		if ($this.hasClass('emoji')) {
			$("#keyboard").hide();
			$("#gesture").show();
			character = '';
		}

		// Webcam key
		if ($this.hasClass('camera')) {
			$("#keyboard").hide();
			$("#webcam").show();
			init();
			character = '';
		}

        // Punctuation key
        if ($this.hasClass('punctuation')) {
			$('.letter span').toggle();
			punctuation = (punctuation == true) ? false : true;
			return false;
		}

		// Shift key
		if ($this.hasClass('shift')) {
            if (punctuation) {
                character = '';
                return false;
            }
            
            if (capslock == true) {
                $('.capslock span').toggle();
            }

			$('.letter').toggleClass('uppercase');
			
			shift = (shift === true) ? false : true;
			capslock = false;
			return false;
		}
		
		// Caps lock
		if ($this.hasClass('capslock')) {
            if (punctuation) {
                character = '';
                return false;
            }

			$('.letter').toggleClass('uppercase');
            $('.capslock span').toggle();
			capslock = true;
			return false;
		}
		
		// Delete
		if ($this.hasClass('delete')) {
			var html = $write.html();
			$write.html(html.substr(0, html.length - 1));
			return false;
		}

        // Return
		if ($this.hasClass('return')) {
            $('textarea').html('');
            return false;
        }

		// Special characters
        if ($this.hasClass('letter')) character = $('span:visible', $this).html();
		if ($this.hasClass('space')) character = ' ';
		
		// Uppercase letter
		if ($this.hasClass('uppercase')) character = character.toUpperCase();
		
		// Remove shift once a key is clicked.
		if (shift === true) {
			if (capslock === false) $('.letter').toggleClass('uppercase');
			
			shift = false;
		}
		// Add the character
		$write.html($write.html() + character);
	});

	$('#gesture').fancygestures(function (data) {
		$write.html($write.html() + data);
		$("#keyboard").show();
		$("#gesture").hide();
	});

	const URL = "https://teachablemachine.withgoogle.com/models/1-UXU4NCp/";
    let model, webcamInit, webcam, maxPredictions, predictCounter, currGuess, pastGuess;

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
        webcam = new tmImage.Webcam(200, 200, flip); // width, height, flip
		await webcam.setup(); // request access to the webcam
		webcam.play();
        window.requestAnimationFrame(loop);

        // append elements to the DOM
		document.getElementById('webcam').appendChild(webcam.canvas);
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
		data = '';
		switch(currGuess) {
			case 0:
				data = "&#128077";
				break;
			case 1:
				data = "&#128075";
				break;
			case 2:
				data = "&#9995";
				break;
			case 3:	
				data = "&#128076";
				break;
			case 4:
				data = "&#128078";
				$("#keyboard").show();
				$("#webcam").hide();
				webcam.stop();
				document.getElementById('webcam').removeChild(webcam.canvas);
				break; 
			case 5: 
				data = '';
				break;
		} 
		if (data == pastGuess) return;
		pastGuess = data;
		$write.html($write.html() + data);
		predictCounter = 0;
		return;
	}
});