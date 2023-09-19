#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <SDL.h>

using json = nlohmann::json;

// Fonction de rappel pour stocker la réponse HTTP
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

int main() {
    // Initialiser libcurl
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Erreur lors de l'initialisation de libcurl." << std::endl;
        return 1;
    }

    // URL à partir de laquelle on obtient le JSON
    std::string url = "https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol=IBM&interval=5min&apikey=WN32ZXLWA3QW33P1";

    // Initialiser la réponse
    std::string response;

    // Configurer la requête HTTP GET
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Exécute la requête HTTP GET
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Erreur lors de l'exécution de la requête : " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return 1;
    }

    // Fermer libcurl
    curl_easy_cleanup(curl);

    // Analyser la réponse JSON
    json data = json::parse(response);

    // Extraction des données pour les graphiques
    std::vector<std::string> timestamps;
    std::vector<float> openPrices;
    std::vector<float> closePrices;
    std::vector<float> highs;
    std::vector<float> lows;
    std::vector<int> volumes;
    std::map<std::string, json> time_series_map = data["Time Series (5min)"];

    for (const auto& entry : time_series_map) {
        timestamps.push_back(entry.first);
        json data = entry.second;
        openPrices.push_back(std::stof(std::string(data["1. open"])));
        highs.push_back(std::stof(std::string(data["2. high"])));
        lows.push_back(std::stof(std::string(data["3. low"])));
        closePrices.push_back(std::stof(std::string(data["4. close"])));
        volumes.push_back(std::stoi(std::string(data["5. volume"])));
    }

    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Erreur lors de l'initialisation de SDL : " << SDL_GetError() << std::endl;
        return 1;
    }

    // Créer une fenêtre SDL
    SDL_Window* window = SDL_CreateWindow("Graphiques Prix Ouverture/Fermeture/Volume/high/low",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Erreur lors de la création de la fenêtre SDL : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Créer un rendu SDL
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Erreur lors de la création du rendu SDL : " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool quit = false;
    SDL_Event e;

    // les dimensions du graphique
    int graphWidth = 800;
    int graphHeight = 400;
    int graphX = 50;
    int graphY = 100;

    // Trouver les plages de données maximales pour adapter l'échelle du graphique
    float maxPrice = *std::max_element(openPrices.begin(), openPrices.end());
    float maxClosePrice = *std::max_element(closePrices.begin(), closePrices.end());
    float maxHigh = *std::max_element(highs.begin(), highs.end());
    float maxLow = *std::max_element(lows.begin(), lows.end());
    int maxVolume = *std::max_element(volumes.begin(), volumes.end());


    // Boucle principale SDL
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // les marges pour le graphique.
        const int marginX = 50;
        const int marginY = 50;

        // Trouver les valeurs minimales et maximales de l'heure et des données Open, Close.
        float minTime = 0;
        float maxTime = timestamps.size() - 1;
        float minValue = 0;

        // Obtenir les dimensions de la fenêtre SDL
        int windowHeight, windowWidth;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        // Dessiner les axes du graphique.
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // blanc
        SDL_RenderClear(renderer); // Effacer le rendu précédent
        SDL_RenderDrawLine(renderer, marginX, marginY, marginX, windowHeight - marginY); // Axe vertical
        SDL_RenderDrawLine(renderer, marginX, windowHeight - marginY, windowWidth - marginX, windowHeight - marginY); // Axe horizontal

        // Dessiner les graphiques en utilisant des lignes pour relier les points.
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Couleur rouge pour le graphique
        for (size_t i = 1; i < timestamps.size(); ++i) {
            float x1 = marginX + (i - 1) * (graphWidth - 2 * marginX) / (maxTime - minTime);
            float x2 = marginX + i * (graphWidth - 2 * marginX) / (maxTime - minTime);

            // Dessine OpenPrice
            float openPriceY1 = graphY + (graphHeight - 2 * marginY) * (maxPrice - openPrices[i - 1]) / (maxPrice - minValue);
            float openPriceY2 = graphY + (graphHeight - 2 * marginY) * (maxPrice - openPrices[i]) / (maxPrice - minValue);
            SDL_RenderDrawLine(renderer, x1, openPriceY1, x2, openPriceY2);
            //Dessine ClosePrice
            float closePriceY1 = graphY + (graphHeight - 2 * marginY) * (maxClosePrice - closePrices[i - 1]) / (maxPrice - minValue);
            float closePriceY2 = graphY + (graphHeight - 2 * marginY) * (maxClosePrice - closePrices[i]) / (maxPrice - minValue);
            SDL_RenderDrawLine(renderer, x1, closePriceY1, x2, closePriceY2);
            //Dessine high
            float highY1 = graphY + (graphHeight - 2 * marginY) * (maxHigh - highs[i - 1]) / (maxHigh - minValue);
            float highY2 = graphY + (graphHeight - 2 * marginY) * (maxHigh - highs[i]) / (maxHigh - minValue);
            SDL_RenderDrawLine(renderer, x1, highY1, x2, highY2);
            //Dessine low
            float lowY1 = graphY + (graphHeight - 2 * marginY) * (maxLow - lows[i - 1]) / (maxLow - minValue);
            float lowY2 = graphY + (graphHeight - 2 * marginY) * (maxLow - lows[i]) / (maxLow - minValue);
            SDL_RenderDrawLine(renderer, x1, lowY1, x2, lowY2);
            //Dessine volume
            float volumeY1 = graphY + (graphHeight - 2 * marginY) * (maxVolume - volumes[i - 1]) / (maxVolume - minValue);
            float volumeY2 = graphY + (graphHeight - 2 * marginY) * (maxVolume - volumes[i]) / (maxVolume - minValue);
            SDL_RenderDrawLine(renderer, x1, volumeY1, x2, volumeY2);
        }

        // Mettre à jour l'affichage.
        SDL_RenderPresent(renderer);
    }

    // Libérer les ressources SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
