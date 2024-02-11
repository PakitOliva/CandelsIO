var jsonDataOld = {
	"modules": [
		[10, 4, 33, 2, 6, 0, 0, 1],
		[0, 0, 32, 6, 85, 4, 2, 0],
		[2, 25, 0, 1, 30, 2, 74, 0]
	]
}

// Función para llenar la tabla con los datos del JSON
function llenarTabla() {
	var tabla = document.getElementById("chandelierTable");
	var tbody = tabla.getElementsByTagName("tbody")[0];
	var jsonData;

	// Crear una instancia de XMLHttpRequest
	var xhr = new XMLHttpRequest();
	// URL
	var url = '/getChandelier';
	// Configurar la petición
	xhr.open('GET', url, true);
	// Configurar la función de callback para manejar la respuesta
	xhr.onload = function () {
		if (xhr.status >= 200 && xhr.status < 300) {
			// La petición fue exitosa
			jsonData = JSON.parse(xhr.responseText);
			console.log(jsonData);

			var modules = jsonData.modules;
			if (modules.length > tbody.rows.length) {
				//Si nos faltan filas, añadimos
				var numRows = modules.length - tbody.rows.length;
				for (var i = 0; i < numRows; i++) {
					tbody.insertRow();
				}
			} else if (modules.length < tbody.rows.length) {
				//Si nos sobran filas, las eliminamos
				var numRows = tbody.rows.length - modules.length;
				for (var i = 0; i < numRows; i++) {
					tbody.deleteRow(0);
				}
			}

			for (var i = 0; i < tbody.rows.length; i++) {
				var row = tbody.rows[i];
				var module = modules[i];

				if (module.length > row.cells.length) {
					//Si nos faltan celdas, añadimos
					var numCells = module.length - row.cells.length;
					for (var j = 0; j < numCells; j++) {
						row.insertCell();
					}
				} else if (module.length < row.cells.length) {
					//Si nos sobran celdas, las eliminamos
					var numCells = row.cells.length - module.length;
					for (var j = 0; j < numCells; j++) {
						row.deleteCell(0);
					}
				}

				for (var j = 0; j < row.cells.length; j++) {
					if (module[j] > 0)
						row.cells[j].innerHTML = "<div class=\"led encendido\"></div> <span> " + module[j] + " min.</span>";
					else
						row.cells[j].innerHTML = "<div class=\"led\"></div>";
				}
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