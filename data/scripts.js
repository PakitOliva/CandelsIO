// Función para llenar la tabla con los datos del JSON
function llenarTabla() {
	var tabla = document.getElementById("modulesTable");
	var tbody = tabla.getElementsByTagName("tbody")[0];
	var deviceId = document.getElementById("deviceId");
	var jsonData;

	// Crear una instancia de XMLHttpRequest
	var xhr = new XMLHttpRequest();
	// URL
	var url = '/getModules';
	// Configurar la petición
	xhr.open('GET', url, true);
	// Configurar la función de callback para manejar la respuesta
	xhr.onload = function () {
		if (xhr.status >= 200 && xhr.status < 300) {
			// La petición fue exitosa
			jsonData = JSON.parse(xhr.responseText);
			console.log(jsonData);
			deviceId.innerHTML= "ID device: " + jsonData.deviceId;
			var modules = jsonData.modules;
			for (var i = 0; i < modules.length; i++) {
				var fila = tbody.insertRow(i);
				var celdaId = fila.insertCell(0);
				var celdaNombre = fila.insertCell(1);
				var ipAP = fila.insertCell(2);
				var ipStation = fila.insertCell(3);
		
				celdaId.innerHTML = modules[i].name;
				celdaNombre.innerHTML = "<a href=\""+modules[i].url+"\">Conectarse</a>";
				ipAP.innerHTML = modules[i].ipAddr;
				ipStation.innerHTML = modules[i].stationIpAddr;
				// Añade más celdas según la estructura de tu JSON
			}
		} else {
			// La petición falló
			console.error('Error al realizar la petición:', xhr.statusText);
			alert('Error al realizar la petición');
		}
	};

	// Configurar la función de callback para manejar errores de red
	xhr.onerror = function () {
		console.error('Error de red al realizar la petición');
		alert('Error de red al realizar la petición');
	};

	// Enviar la petición
	xhr.send();
}

// Llama a la función para llenar la tabla al cargar la página
window.onload = llenarTabla;