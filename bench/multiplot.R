# R program to plot CSV results of a benchmark program to a single image file

library(readr)
library(ggplot2)
library(reshape2)
library(scales)

args = commandArgs(trailingOnly = TRUE)

if (length(args) < 2)
{
    stop("Two arguments must be supplied (title of diagram, file pattern).", call. = FALSE)
}

scientific_10 <- function(x)
{
    parse(text = gsub("e", " %*% 10^", scales::scientific_format()(x)))
}

files = list.files(pattern = args[2], full.names = TRUE)

for (file in files)
{
    data = read_csv(file)
    transformed_data = melt(data, id = "quantity")

    p <- ggplot(transformed_data, aes(x = quantity, y = value, col = variable)) +
        geom_line() +
        geom_point() +
        scale_x_continuous(label = scientific_10) +
        scale_y_continuous(labels = comma) +
        labs(title = args[1], x = "Array Length (Bytes)", y = "Average Time (Microseconds)") +
        theme(legend.title = element_blank()) + theme(plot.title = element_text(hjust = 0.5))

    file = gsub(".csv", ".jpg", file)

    ggsave(file, width = 10, height = 5, dpi = 100, units = "in", device = "jpg")
}
