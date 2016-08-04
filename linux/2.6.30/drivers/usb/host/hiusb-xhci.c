/*
 * XHCI HCD (Host Controller Driver) for USB.
 *
 * (C) Copyright 2013 HSAN
 *
 * This file is licenced under the GPL.
 */

#include <linux/platform_device.h>

extern int usb_disabled(void);

extern void hiusb_start_hcd(void);
extern void hiusb_stop_hcd(void);


#define HI_DMA_BIT_MASK(n)	(((n) == 64) ? ~0ULL : ((1ULL<<(n))-1))


static int hiusb_xhci_setup(struct usb_hcd *hcd)
{
	int ret = xhci_init(hcd);

	return ret;
}

static const struct hc_driver hiusb_xhci_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "HIUSB XHCI",
	.hcd_priv_size		= sizeof(struct xhci_hcd),

	/*
	 * generic hardware linkage
	 */
	.irq			= xhci_irq,
	.flags			= HCD_MEMORY | HCD_USB2,

	/*
	 * basic lifecycle operations
	 *
	 * FIXME -- xhci_init() doesn't do enough here.
	 * See xhci-ppc-soc for a complete implementation.
	 */
	.reset			= hiusb_xhci_setup,
	.start			= xhci_run,
	.stop			= xhci_stop,
	.shutdown		= xhci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue		= xhci_urb_enqueue,
	.urb_dequeue		= xhci_urb_dequeue,
	//.endpoint_disable	= xhci_endpoint_disable,
    .endpoint_reset  = xhci_endpoint_reset,

	/*
	 * scheduling support
	 */
	.get_frame_number	= xhci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data	= xhci_hub_status_data,
	.hub_control		= xhci_hub_control,
	//.bus_suspend		= xhci_bus_suspend,
	//.bus_resume		= xhci_bus_resume,
	//.relinquish_port	= xhci_relinquish_port,
	//.port_handed_over	= xhci_port_handed_over,

///	.clear_tt_buffer_complete	= xhci_clear_tt_buffer_complete,
};

static int hiusb_xhci_hcd_drv_probe(struct platform_device *pdev)
{
	struct usb_hcd *hcd;
	struct xhci_hcd *xhci;
	struct resource *res;
	int ret;

	if (usb_disabled())
		return -ENODEV;

	if (pdev->resource[1].flags != IORESOURCE_IRQ) {
		pr_debug("resource[1] is not IORESOURCE_IRQ");
		return -ENOMEM;
	}
	hcd = usb_create_hcd(&hiusb_xhci_hc_driver, &pdev->dev, "hiusb-xhci");
	if (!hcd)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
		pr_debug("request_mem_region failed");
		ret = -EBUSY;
		goto err1;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		pr_debug("ioremap failed");
		ret = -ENOMEM;
		goto err2;
	}

	hiusb_start_hcd();

	xhci = hcd_to_xhci(hcd);
	xhci->cap_regs = hcd->regs;
	xhci->op_regs = hcd->regs + HC_LENGTH(readl(&xhci->cap_regs->hc_capbase));
	/* cache this readonly data; minimize chip reads */
	xhci->hcs_params1 = readl(&xhci->cap_regs->hcs_params1);
	xhci->hcs_params2 = readl(&xhci->cap_regs->hcs_params2);
	xhci->hcs_params3 = readl(&xhci->cap_regs->hcs_params3);
    xhci->hcc_params = readl(&xhci->cap_regs->hcc_params);

	ret = usb_add_hcd(hcd, pdev->resource[1].start,
			  IRQF_DISABLED | IRQF_SHARED);
	if (ret == 0) {
		platform_set_drvdata(pdev, hcd);
		return ret;
	}

	hiusb_stop_hcd();
	iounmap(hcd->regs);
err2:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
err1:
	usb_put_hcd(hcd);
	return ret;
}

static int hiusb_xhci_hcd_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
	hiusb_stop_hcd();
	platform_set_drvdata(pdev, NULL);

	return 0;
}

//#ifdef CONFIG_PM by luocheng 20120830
#if 0
static int hiusb_xhci_hcd_drv_suspend(struct device *dev)
{
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	unsigned long flags;
	int rc;

	rc = 0;

	if (time_before(jiffies, xhci->next_statechange))
		msleep(10);

	/* Root hub was already suspended. Disable irq emission and
	 * mark HW unaccessible.  The PM and USB cores make sure that
	 * the root hub is either suspended or stopped.
	 */
	spin_lock_irqsave(&xhci->lock, flags);
	xhci_prepare_ports_for_controller_suspend(xhci);
	xhci_writel(xhci, 0, &xhci->regs->intr_enable);
	(void)xhci_readl(xhci, &xhci->regs->intr_enable);

	clear_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);

	hiusb_stop_hcd();
	spin_unlock_irqrestore(&xhci->lock, flags);

	// could save FLADJ in case of Vaux power loss
	// ... we'd only use it to handle clock skew

	return rc;
}

static int hiusb_xhci_hcd_drv_resume(struct device *dev)
{
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);

	hiusb_start_hcd();

	// maybe restore FLADJ

	if (time_before(jiffies, xhci->next_statechange))
		msleep(100);

	/* Mark hardware accessible again as we are out of D3 state by now */
	set_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);

	/* If CF is still set, we maintained PCI Vaux power.
	 * Just undo the effect of xhci_pci_suspend().
	 */
	if (xhci_readl(xhci, &xhci->regs->configured_flag) == FLAG_CF) {
		int	mask = INTR_MASK;

		xhci_prepare_ports_for_controller_resume(xhci);
		if (!hcd->self.root_hub->do_remote_wakeup)
			mask &= ~STS_PCD;
		xhci_writel(xhci, mask, &xhci->regs->intr_enable);
		xhci_readl(xhci, &xhci->regs->intr_enable);
		return 0;
	}

	xhci_dbg(xhci, "lost power, restarting\n");
	usb_root_hub_lost_power(hcd->self.root_hub);

	/* Else reset, to cope with power loss or flush-to-storage
	 * style "resume" having let BIOS kick in during reboot.
	 */
	(void) xhci_halt(xhci);
	(void) xhci_reset(xhci);

	/* emptying the schedule aborts any urbs */
	spin_lock_irq(&xhci->lock);
	if (xhci->reclaim)
		end_unlink_async(xhci);
	xhci_work(xhci);
	spin_unlock_irq(&xhci->lock);

	xhci_writel(xhci, xhci->command, &xhci->regs->command);
	xhci_writel(xhci, FLAG_CF, &xhci->regs->configured_flag);
	xhci_readl(xhci, &xhci->regs->command);	/* unblock posted writes */

	/* here we "know" root ports should always stay powered */
	xhci_port_power(xhci, 1);

	hcd->state = HC_STATE_SUSPENDED;

	return 0;
}

static const struct dev_pm_ops hiusb_xhci_pmops = {
	.suspend	= hiusb_xhci_hcd_drv_suspend,
	.resume		= hiusb_xhci_hcd_drv_resume,
};

#define HIUSB_XHCI_PMOPS &hiusb_xhci_pmops

#else
#define HIUSB_XHCI_PMOPS NULL
#endif

static struct platform_driver hiusb_xhci_hcd_driver = {
	.probe		= hiusb_xhci_hcd_drv_probe,
	.remove		= hiusb_xhci_hcd_drv_remove,
	.shutdown	= usb_hcd_platform_shutdown,
	.driver = {
		.name	= "hiusb-xhci",
		.owner	= THIS_MODULE,
		.pm	= HIUSB_XHCI_PMOPS,
	}
};

MODULE_ALIAS("platform:hiusb-xhci");

/*****************************************************************************/


static struct resource hiusb_xhci_res[] = {
        [0] = {
                .start = CONFIG_USB_HSAN_XHCI_IOBASE,
                .end   = CONFIG_USB_HSAN_XHCI_IOBASE + CONFIG_USB_HSAN_XHCI_IOSIZE - 1,
                .flags = IORESOURCE_MEM,
        },
        [1] = {
                .start = CONFIG_USB_HSAN_XHCI_IRQNUM,
                .end   = CONFIG_USB_HSAN_XHCI_IRQNUM,
                .flags = IORESOURCE_IRQ,
        },
};

static void usb_xhci_platdev_release (struct device *dev)
{
        /* These don't need to do anything because the pdev structures are
         * statically allocated. */
}

static u64 usb_dmamask = HI_DMA_BIT_MASK(32);
static struct platform_device hiusb_xhci_platdev= {
        .name   = "hiusb-xhci",
        .id     = 0,
        .dev    = {
                .platform_data  = NULL,
                .dma_mask = &usb_dmamask,
                .coherent_dma_mask = HI_DMA_BIT_MASK(32),
                .release = usb_xhci_platdev_release,
        },
        .num_resources = ARRAY_SIZE(hiusb_xhci_res),
        .resource       = hiusb_xhci_res,
};
